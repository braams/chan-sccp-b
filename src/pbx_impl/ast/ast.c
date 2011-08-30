
/*!
 * \file 	sccp_ast.c
 * \brief 	SCCP PBX Asterisk Wrapper Class
 * \author 	Diederik de Groot <ddegroot [at] users.sourceforge.net>
 * \note	Reworked, but based on chan_sccp code.
 *        	The original chan_sccp driver that was made by Zozo which itself was derived from the chan_skinny driver.
 *        	Modified by Jan Czmok and Julien Goodwin
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *		See the LICENSE file at the top of the source tree.
 *
 * $Date: 2010-10-23 20:04:30 +0200 (Sat, 23 Oct 2010) $
 * $Revision: 2044 $  
 */
#include "../../config.h"
#include "../../common.h"

SCCP_FILE_VERSION(__FILE__, "$Revision: 2269 $")

/************************************************************************************************************ CALLERID **/

/*
 * \brief get callerid_name from pbx
 * \param sccp_channle Asterisk Channel
 * \param cid name result
 * \return parse result
 */

/*
static int sccp_wrapper_asterisk_callerid_name(const sccp_channel_t * channel, char **cid_name)
{
	int return_val = 0;

#ifdef ASTERISK_CONF_1_8
	// impl 1.8
#else
	PBX_CHANNEL_TYPE *pbx_channel = channel->owner;

#    ifndef CS_AST_CHANNEL_HAS_CID						//ast1.2
	char *name, *number, *cidtmp;

	if (pbx_channel->callerid && (cidtmp = strdup(pbx_channel->callerid))) {
		ast_callerid_parse(cidtmp, &name, &number);
		if (name && strlen(name) > 0) {
			*cid_name = strdup(name);
			return_val = 1;
		}
		if (cidtmp)
			ast_free(cidtmp);
		cidtmp = NULL;

		if (name)
			ast_free(name);
		name = NULL;

		if (number)
			ast_free(number);
		number = NULL;
	}
#    else
	if (pbx_channel->cid.cid_name && strlen(pbx_channel->cid.cid_name) > 0) {
		*cid_name = strdup(pbx_channel->cid.cid_name);
		return_val = 1;
	}
#    endif									// CS_AST_CHANNEL_HAS_CID
#endif
	return return_val;
}
*/

/*
 * \brief get callerid_name from pbx
 * \param ast_chan Asterisk Channel
 * \return char * with the callername
 */

/*
static int sccp_wrapper_asterisk_callerid_number(const sccp_channel_t * channel, char **cid_number)
{
	int return_val = 0;

#ifdef ASTERISK_CONF_1_8
	// impl 1.8
#else
	PBX_CHANNEL_TYPE *pbx_channel = channel->owner;

#    ifndef CS_AST_CHANNEL_HAS_CID						// ast1.2
	char *name, *number, *cidtmp;

	if (pbx_channel->callerid && (cidtmp = strdup(pbx_channel->callerid))) {
		ast_callerid_parse(cidtmp, &name, &number);
		if (number && strlen(number) > 0) {
			*cid_number = strdup(number);
			return_val = 1;
		}
		if (cidtmp)
			ast_free(cidtmp);
		cidtmp = NULL;

		if (name)
			ast_free(name);
		name = NULL;

		if (number)
			ast_free(number);
		number = NULL;
	}
#    else

	if (pbx_channel->cid.cid_num && strlen(pbx_channel->cid.cid_num) > 0) {
		*cid_number = strdup(pbx_channel->cid.cid_num);
		return_val = 1;
	}
#    endif									// CS_AST_CHANNEL_HAS_CID
#endif

	return return_val;
}
*/

/************************************************************************************************************* CHANNEL **/

/*
 * \brief itterate through locked pbx channels
 * \note replacement for ast_channel_walk_locked
 * \param ast_chan Asterisk Channel
 * \return ast_chan Locked Asterisk Channel
 *
 * \todo implement pbx_channel_walk_locked or replace
 * \warning marcello is this the right implementation for pbx_channel_walk_locked. Not sure if i replaced the iterator correctly
 */
PBX_CHANNEL_TYPE *pbx_channel_walk_locked(PBX_CHANNEL_TYPE * target)
{
#if ASTERISK_VERSION_NUMBER >= 10800
	struct ast_channel_iterator *iter = NULL; 
	if (!target) {
		if (!(iter = ast_channel_iterator_all_new())) {
			return NULL;  
		}
	} 
       	target = ast_channel_iterator_next(iter);
	if (!ast_channel_unref(target)) {
		ast_channel_lock(target);
		return target;
	} else {
 		if (iter) {
			ast_channel_iterator_destroy(iter);
		}
		return NULL;
	}
#else
	return ast_channel_walk_locked(target);
#endif
}

/*
 * \brief itterate through locked pbx channels and search using specified match function
 * \param is_match match function
 * \param data paremeter data for match function
 * \return pbx_channel Locked Asterisk Channel
 */
PBX_CHANNEL_TYPE *pbx_channel_search_locked(int (*is_match)(PBX_CHANNEL_TYPE *, void *),void *data){
//#ifdef ast_channel_search_locked
#if ASTERISK_VERSION_NUMBER < 10800
	return ast_channel_search_locked(is_match, data);
#else
        boolean_t matched=FALSE;
        PBX_CHANNEL_TYPE *pbx_channel = NULL;
  
  	struct ast_channel_iterator *iter = NULL;
        if (!(iter = ast_channel_iterator_all_new())) {
     	        return NULL;  
        }
 
 	for (; iter && (pbx_channel = ast_channel_iterator_next(iter)); ast_channel_unref(pbx_channel)) {
  		pbx_channel_lock(pbx_channel);
                if (is_match(pbx_channel, data)) {
                        matched=TRUE;
                        break;
                }
   		pbx_channel_unlock(pbx_channel);
        }

        if (iter) {
	        ast_channel_iterator_destroy(iter);
        }

	if (matched)
		return pbx_channel;
	else 
	        return NULL;

#endif
}


/************************************************************************************************************** CONFIG **/

/*
 * \brief Add a new rule to a list of HAs
 * \note replacement for ast_append_ha
 * \param sense Sense / Key
 * \param stuff Value / Stuff to Add
 * \param path list of HAs
 * \param error Error as int
 * \return The head of the HA list
 */
struct ast_ha *pbx_append_ha(NEWCONST char *sense, const char *stuff, struct ast_ha *path, int *error)
{
#if ASTERISK_VERSION_NUMBER < 10600
	return ast_append_ha(sense, stuff, path);
#else
	return ast_append_ha(sense, stuff, path, error);
#endif										// ASTERISK_VERSION_NUMBER
}

/*
 * \brief Register a new context
 * \note replacement for ast_context_find_or_create
 *
 * This will first search for a context with your name.  If it exists already, it will not
 * create a new one.  If it does not exist, it will create a new one with the given name
 * and registrar.
 *
 * \param extcontexts pointer to the ast_context structure pointer
 * \param name name of the new context
 * \param registrar registrar of the context
 * \return NULL on failure, and an ast_context structure on success
 */
struct ast_context *pbx_context_find_or_create(struct ast_context **extcontexts, struct ast_hashtab *exttable, const char *name, const char *registrar)
{
#if ASTERISK_VERSION_NUMBER < 10600
	return ast_context_find_or_create(extcontexts, name, registrar);
#else
	return ast_context_find_or_create(extcontexts, exttable, name, registrar);
#endif										// ASTERISK_VERSION_NUMBER
}

/*!
 * \brief Load a config file
 *
 * \param filename path of file to open.  If no preceding '/' character,
 * path is considered relative to AST_CONFIG_DIR
 * \param who_asked The module which is making this request.
 * \param flags Optional flags:
 * CONFIG_FLAG_WITHCOMMENTS - load the file with comments intact;
 * CONFIG_FLAG_FILEUNCHANGED - check the file mtime and return CONFIG_STATUS_FILEUNCHANGED if the mtime is the same; or
 * CONFIG_FLAG_NOCACHE - don't cache file mtime (main purpose of this option is to save memory on temporary files).
 *
 * \details
 * Create a config structure from a given configuration file.
 *
 * \return an ast_config data structure on success
 * \retval NULL on error
 */
struct ast_config *pbx_config_load(const char *filename, const char *who_asked, struct ast_flags flags)
{
#if ASTERISK_VERSION_NUMBER < 10600
	return ast_config_load(filename);
#else
	return ast_config_load2(filename, who_asked, flags);
#endif										// ASTERISK_VERSION_NUMBER
}





/*!
 * \brief Get/Create new config variable
 * \note replacement for ast_variable_new
 * \param name Variable Name as char
 * \param value Variable Value as char
 * \param filename Filename
 * \return The return value is struct ast_variable.
 */
PBX_VARIABLE_TYPE *pbx_variable_new(PBX_VARIABLE_TYPE *v)
{
#if ASTERISK_VERSION_NUMBER >= 10600
	return ast_variable_new(v->name, v->value, v->file);
#else
	return ast_variable_new(v->name, v->value);
#endif										// ASTERISK_VERSION_NUMBER
}

/******************************************************************************************************** NET / SOCKET **/

/*
 * \brief thread-safe replacement for inet_ntoa()
 * \note replacement for ast_pbx_inet_ntoa
 * \param ia In Address / Source Address
 * \return Address as char
 */
const char *pbx_inet_ntoa(struct in_addr ia)
{
#if ASTERISK_VERSION_NUMBER < 10400
	char iabuf[INET_ADDRSTRLEN];

	return ast_inet_ntoa(iabuf, sizeof(iabuf), ia);
#else
	return ast_inet_ntoa(ia);
#endif										// ASTERISK_VERSION_NUMBER
}

#if ASTERISK_VERSION_NUMBER < 10400

/* BackPort of ast_str2cos & ast_str2cos for asterisk 1.2*/
struct dscp_codepoint {
	char *name;
	unsigned int space;
};

static const struct dscp_codepoint dscp_pool1[] = {
	{"CS0", 0x00},
	{"CS1", 0x08},
	{"CS2", 0x10},
	{"CS3", 0x18},
	{"CS4", 0x20},
	{"CS5", 0x28},
	{"CS6", 0x30},
	{"CS7", 0x38},
	{"AF11", 0x0A},
	{"AF12", 0x0C},
	{"AF13", 0x0E},
	{"AF21", 0x12},
	{"AF22", 0x14},
	{"AF23", 0x16},
	{"AF31", 0x1A},
	{"AF32", 0x1C},
	{"AF33", 0x1E},
	{"AF41", 0x22},
	{"AF42", 0x24},
	{"AF43", 0x26},
	{"EF", 0x2E},
};

int pbx_str2tos(const char *value, unsigned int *tos)
{
	int fval;

	unsigned int x;

	if (sscanf(value, "%30i", &fval) == 1) {
		*tos = fval & 0xFF;
		return 0;
	}

	for (x = 0; x < ARRAY_LEN(dscp_pool1); x++) {
		if (!strcasecmp(value, dscp_pool1[x].name)) {
			*tos = dscp_pool1[x].space << 2;
			return 0;
		}
	}

	return -1;
}
#else
int pbx_str2tos(const char *value, unsigned int *tos)
{
	return ast_str2tos(value, tos);
}
#endif										// ASTERISK_VERSION_NUMBER

#if ASTERISK_VERSION_NUMBER < 10600
int pbx_str2cos(const char *value, unsigned int *cos)
{
	int fval;

	if (sscanf(value, "%30d", &fval) == 1) {
		if (fval < 8) {
			*cos = fval;
			return 0;
		}
	}

	return -1;
}
#else
int pbx_str2cos(const char *value, unsigned int *cos)
{
	return ast_str2cos(value, cos);
}
#endif										// ASTERISK_VERSION_NUMBER

/************************************************************************************************************* GENERAL **/

/*! 
 * \brief Simply remove extension from context
 * \note replacement for ast_context_remove_extension
 * 
 * \param context context to remove extension from
 * \param extension which extension to remove
 * \param priority priority of extension to remove (0 to remove all)
 * \param callerid NULL to remove all; non-NULL to match a single record per priority
 * \param matchcid non-zero to match callerid element (if non-NULL); 0 to match default case
 * \param registrar registrar of the extension
 *
 * This function removes an extension from a given context.
 *
 * \retval 0 on success 
 * \retval -1 on failure
 * 
 * @{
 */
int pbx_context_remove_extension(const char *context, const char *extension, int priority, const char *registrar)
{
#if ASTERISK_VERSION_NUMBER >= 10600
	struct pbx_find_info q = {.stacklen = 0 };
	if (pbx_find_extension(NULL, NULL, &q, context, extension, 1, NULL, "", E_MATCH)) {
		return ast_context_remove_extension(context, extension, priority, registrar);
	} else {
		return -1;
	}
#else
	return ast_context_remove_extension(context, extension, priority, registrar);
#endif										// ASTERISK_VERSION_NUMBER
}

/*!   
 * \brief Send ack in manager list transaction
 * \note replacement for astman_send_listack
 * \param context which context to add the ignorpattern to
 * \param ignorepat ignorepattern to set up for the extension
 * \param registrar registrar of the ignore pattern
 */
void pbxman_send_listack(struct mansession *s, const struct message *m, char *msg, char *listflag)
{
#if ASTERISK_VERSION_NUMBER < 10600
	astman_send_ack(s, m, msg);
#else
	astman_send_listack(s, m, msg, listflag);
#endif										// ASTERISK_VERSION_NUMBER
}

/*!
 * \brief Turn on music on hold on a given channel 
 * \note replacement for ast_moh_start
 *
 * \param chan The channel structure that will get music on hold
 * \param mclass The class to use if the musicclass is not currently set on
 *               the channel structure.
 * \param interpclass The class to use if the musicclass is not currently set on
 *                    the channel structure or in the mclass argument.
 *
 * \retval Zero on success
 * \retval non-zero on failure
 */
int pbx_moh_start(PBX_CHANNEL_TYPE * chan, const char *mclass, const char *interpclass)
{
#if ASTERISK_VERSION_NUMBER < 10400
	return ast_moh_start(chan, interpclass);
#else
	return ast_moh_start(chan, mclass, interpclass);
#endif										// ASTERISK_VERSION_NUMBER
}

/***************************************************************************************************************** RTP **/
/*
sccp_extension_status_t sccp_wrapper_asterisk_extensionStatus(const sccp_channel_t * channel)
{
	PBX_CHANNEL_TYPE *pbx_channel = channel->owner;

	int ignore_pat = ast_ignore_pattern(pbx_channel->context, channel->dialedNumber);

	int ext_exist = ast_exists_extension(pbx_channel, pbx_channel->context,
					     channel->dialedNumber, 1, channel->line->cid_num);

	int ext_canmatch = ast_canmatch_extension(pbx_channel, pbx_channel->context,
						  channel->dialedNumber, 1, channel->line->cid_num);

	int ext_matchmore = ast_matchmore_extension(pbx_channel, pbx_channel->context,
						    channel->dialedNumber, 1,
						    channel->line->cid_num);

	sccp_log(1) (VERBOSE_PREFIX_1 "SCCP: extension helper says that:\n" "ignore pattern  : %d\n" "exten_exists    : %d\n" "exten_canmatch  : %d\n" "exten_matchmore : %d\n", ignore_pat, ext_exist, ext_canmatch, ext_matchmore);
	if (ignore_pat) {
		return SCCP_EXTENSION_NOTEXISTS;
	} else if (ext_exist) {
		if (ext_canmatch)
			return SCCP_EXTENSION_EXACTMATCH;
		else
			return SCCP_EXTENSION_MATCHMORE;
	}

	return SCCP_EXTENSION_NOTEXISTS;
}

//sccp_parkresult_t sccp_wrapper_asterisk_park(const sccp_channel_t * channel, const sccp_channel_t * host, int timeout, int *parkinglot)
sccp_parkresult_t sccp_wrapper_asterisk_park(const sccp_channel_t * channel, const sccp_channel_t * host_channel, int timeout, const char *parkinglot, int *extout)
{
	int result;

	PBX_CHANNEL_TYPE *pbx_channel = channel->owner;

	PBX_CHANNEL_TYPE *pbx_host_channel = host_channel->owner;

#if ASTERISK_VERSION_NUMBER >= 10800
	result = ast_park_call(pbx_channel, pbx_host_channel, timeout, parkinglot, extout);
#else
	result = ast_park_call(pbx_channel, pbx_host_channel, timeout, extout);
#endif
	if (result == 0)
		return PARK_RESULT_SUCCESS;
	else
		return PARK_RESULT_FAIL;
}

int sccp_wrapper_asterisk_setCallState(const sccp_channel_t * channel, int state)
{
	if (channel && channel->owner) {
		ast_setstate(channel->owner, state);
		sccp_log((DEBUGCAT_CHANNEL)) (VERBOSE_PREFIX_3 "%s: Set asterisk state %s (%d) for call %d\n", DEV_ID_LOG(sccp_channel_getDevice(channel)), ast_state2str(state), state, channel->callid);
	}
	return 0;
}
*/

/****************************************************************************************************** CODEC / FORMAT **/

/*!
 * \brief Convert a ast_codec (fmt) to a skinny codec (enum)
 *
 * \param fmt Format as ast_format_type
 *
 * \return skinny_codec 
 */
skinny_codec_t pbx_codec2skinny_codec(ast_format_type fmt)
{
	uint32_t i;

	for (i = 1; i < ARRAY_LEN(skinny2pbx_codec_maps); i++) {
		if (skinny2pbx_codec_maps[i].pbx_codec == (uint64_t) fmt) {
			return skinny2pbx_codec_maps[i].skinny_codec;
		}
	}
	return 0;
}

/*!
 * \brief Convert a skinny_codec (enum) to an ast_codec (fmt)
 *
 * \param skinny_codec Skinny Codec (enum)
 *
 * \return fmt Format as ast_format_type
 */
ast_format_type skinny_codec2pbx_codec(skinny_codec_t codec)
{
	uint32_t i;

	for (i = 1; i < ARRAY_LEN(skinny2pbx_codec_maps); i++) {
		if (skinny2pbx_codec_maps[i].skinny_codec == codec) {
			return skinny2pbx_codec_maps[i].pbx_codec;
		}
	}
	return 0;
}

/*
// \todo check bitwise operator (not sure) - DdG
skinny_codec_t *pbx_codecs2skinny_codecs(int codecs)
{
        skinny_codec_t *res=NULL;
        unsigned int i; 
        int pbx_codec, sccp_codec_entry=0;
	for (i = 1; i < ARRAY_LEN(skinny2pbx_codec_maps); i++) {
		if (SKINNY_MAX_CAPABILITIES >= sccp_codec_entry) {
                        pbx_codec=skinny2pbx_codec_maps[i].pbx_codec;
                        if ((codecs & pbx_codec) == pbx_codec){		//bitwise match
                                res[sccp_codec_entry++]=skinny2pbx_codec_maps[i].skinny_codec;
                        } else {
                                ast_log(LOG_WARNING, "No matching SCCP codec found for Ast_codec:%d\n", skinny2pbx_codec_maps[i].pbx_codec);
                        }
		} else {
			ast_log(LOG_WARNING, "Device Codec Overflow: Reached Device Max Capabilities");
		}
	}
	return res;
}
*/

/*!
 * \brief Convert an array of skinny_codecs (enum) to a bit array of ast_codecs (fmt)
 *
 * \param skinny_codecs Array of Skinny Codecs
 *
 * \return bit array fmt/Format of ast_format_type (int)
 *
 * \todo check bitwise operator (not sure) - DdG 
 */
int skinny_codecs2pbx_codecs(skinny_codec_t * skinny_codecs)
{
	uint32_t i;
	int res_codec = 0;

	for (i = 1; i < SKINNY_MAX_CAPABILITIES; i++) {
		res_codec |= skinny_codec2pbx_codec(skinny_codecs[i]);
	}
	return res_codec;
}

/*
skinny_codec_t *pbx_codec_pref2sccp_codec_pref(struct ast_codec_pref *prefs)
{
        skinny_codec_t *res=NULL;
        int i, codec, mpl_entry=0;
        
        for(i = 0; i < 32 ; i++) {
		if (SKINNY_MAX_CAPABILITIES >= mpl_entry) {
                        if ((codec = ast_codec_pref_index(prefs,i))) {
                                res[mpl_entry++]=pbx_codec2skinny_codec(codec);
                        }
		} else {
			ast_log(LOG_WARNING, "Device Codec Overflow: Reached Device Max Capabilities");
		}
        }
        return res;
}
*/

/*
struct ast_codec_pref *sccp_codec_pref2pbx_codec_pref(skinny_codec_t *skinny_codecs)
{
        struct ast_codec_pref *res=NULL;
        int i,x;
        for(i = 0; i < SKINNY_MAX_CAPABILITIES; i++) {
		ast_log(LOG_WARNING, "%d: %d => %d\n", i, skinny_codecs[i], skinny_codec2pbx_codec(skinny_codecs[i]));
                if (0!=skinny_codecs[i]) {
        		x=ast_codec_pref_append(res, skinny_codec2pbx_codec(skinny_codecs[i]));
		}
        }        
        return res;
}
*/