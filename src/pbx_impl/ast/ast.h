
/*!
 * \file 	sccp_ast.h
 * \brief 	SCCP PBX Asterisk Header
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
#ifndef __SCCP_PBX_WRAPPER_H
#    define __SCCP_PBX_WRAPPER_H
#    include "../../common.h"

#    if ASTERISK_VERSION_NUMBER >= 10400
#        include <asterisk.h>
#        include <asterisk/abstract_jb.h>
#    endif
#    include <asterisk/utils.h>
#    include <asterisk/threadstorage.h>
#    include <asterisk/strings.h>
#    include <asterisk/pbx.h>
#    include <asterisk/acl.h>
#    include <asterisk/module.h>
#    include <asterisk/options.h>
#    include <asterisk/logger.h>
#    include <asterisk/config.h>
#    include <asterisk/sched.h>
#    include <asterisk/causes.h>
#    include <asterisk/frame.h>
#    include <asterisk/lock.h>
#    include <asterisk/channel.h>
#    include <asterisk/app.h>
#    include <asterisk/callerid.h>
#    include <asterisk/musiconhold.h>
#    include <asterisk/astdb.h>
#    ifdef CS_SCCP_PICKUP
#        include <asterisk/features.h>
#    endif
#    ifdef HAVE_PBX_EVENT
#        include <asterisk/event.h>
#    endif
#    ifndef CS_AST_HAS_TECH_PVT
#        include <asterisk/channel_pvt.h>
#    endif
#    ifdef HAVE_PBX_DEVICESTATE_H
#        include <asterisk/devicestate.h>
#    endif
#    ifndef HAVE_PBX_RTP_ENGINE_H
#         include <asterisk/rtp.h>
#    else
#         include <asterisk/rtp_engine.h>
#    endif
#    ifdef AST_EVENT_IE_CIDNAME
#        include <asterisk/event.h>
#        include <asterisk/event_defs.h>
#    endif
#    ifdef CS_AST_HAS_AST_STRING_FIELD
#        include <asterisk/stringfields.h>
#    endif
#    ifdef CS_MANAGER_EVENTS
#        include <asterisk/manager.h>
#    endif
#    ifdef CS_AST_HAS_ENDIAN
#        include <asterisk/endian.h>
#    endif
#    include <asterisk/translate.h>
#    ifdef CS_DEVSTATE_FEATURE
#      include <asterisk/event_defs.h>
#    endif

#    include "define.h"
#    ifdef ASTERISK_CONF_1_2
#        include "ast102.h"
//#        include "ast10200.h"
#        define PBXVER 10200
#    endif
#    ifdef ASTERISK_CONF_1_4
#        include "ast104.h"
//#        include "ast10400.h"
#        define PBXVER 10400
#    endif
#    ifdef ASTERISK_CONF_1_6
#        include "ast106.h"
/*#        if ASTERISK_VERSION_NUMBER == 10600
#            include "ast10600.h"
#            define PBXVER 10600
#        elif ASTERISK_VERSION_NUMBER == 10601
#            include "ast10601.h"
#            define PBXVER 10601
#        elif ASTERISK_VERSION_NUMBER == 10602
#            include "ast10602.h"
#            define PBXVER 10602
#        endif
*/
#    endif
#    ifdef ASTERISK_CONF_1_8
#        include "ast108.h"
#    endif
#    ifdef ASTERISK_CONF_1_10
#	ifdef ASTERISK_REPOS_LOCATION_TRUNK
#         include "astTrunk.h"
#	else
#         include "ast110.h"
#	endif
/*#        if ASTERISK_VERSION_NUMBER == 11000
#            include "ast11000.h"
#            define PBXVER 11000
#        endif*/
#    endif

extern struct sccp_pbx_cb sccp_pbx;

#    define PBX(x) sccp_pbx.x
#    define AST_MODULE "chan_sccp"

/*!
 * \brief Skinny Codec Mapping
 */
static const struct skinny2pbx_codec_map {
	skinny_codec_t skinny_codec;
	uint64_t pbx_codec;
} skinny2pbx_codec_maps[] = {
	/* *INDENT-OFF* */
	{SKINNY_CODEC_NONE, 		0},
	{SKINNY_CODEC_G711_ALAW_64K, 	AST_FORMAT_ALAW},
	{SKINNY_CODEC_G711_ALAW_56K, 	AST_FORMAT_ALAW},
	{SKINNY_CODEC_G711_ULAW_64K, 	AST_FORMAT_ULAW},
	{SKINNY_CODEC_G711_ULAW_56K, 	AST_FORMAT_ULAW},
	{SKINNY_CODEC_G723_1, 		AST_FORMAT_G723_1},
	{SKINNY_CODEC_G729, 		AST_FORMAT_G729A},
	{SKINNY_CODEC_G729_A, 		AST_FORMAT_G729A},
	{SKINNY_CODEC_GSM, 		AST_FORMAT_GSM},
	{SKINNY_CODEC_H261, 		AST_FORMAT_H261},
	{SKINNY_CODEC_H263, 		AST_FORMAT_H263},
	{SKINNY_CODEC_H263P,	 	AST_FORMAT_H263_PLUS},
	{SKINNY_CODEC_T120, 		AST_FORMAT_T140},
	{SKINNY_CODEC_WIDEBAND_256K,	AST_FORMAT_SLINEAR16},
#    if ASTERISK_VERSION_NUMBER >= 10400
	{SKINNY_CODEC_G726_32K, 	AST_FORMAT_G726_AAL2},
	{SKINNY_CODEC_G726_32K, 	AST_FORMAT_G726},
	{SKINNY_CODEC_G729_B_LOW, 	AST_FORMAT_ILBC},
	{SKINNY_CODEC_G722_64K, 	AST_FORMAT_G722},
	{SKINNY_CODEC_G722_56K, 	AST_FORMAT_G722},
	{SKINNY_CODEC_G722_48K, 	AST_FORMAT_G722},
	{SKINNY_CODEC_H264, 		AST_FORMAT_H264},
#    endif
#    ifdef AST_FORMAT_SIREN7
	{SKINNY_CODEC_G722_1_24K, 	AST_FORMAT_SIREN7},			// should this not be SKINNY_CODEC_G722_1_32K
	{SKINNY_CODEC_G722_1_32K, 	AST_FORMAT_SIREN14},			// should this not be SKINNY_CODEC_G722_1_48K
#    endif
	/* *INDENT-ON* */
};


int set_pbx_callerid(PBX_CHANNEL_TYPE * ast_chan, sccp_callinfo_t * callInfo);
PBX_CHANNEL_TYPE *pbx_channel_walk_locked(PBX_CHANNEL_TYPE * target);
PBX_CHANNEL_TYPE *pbx_channel_search_locked(int (*is_match)(PBX_CHANNEL_TYPE *, void *),void *data);
struct ast_ha *pbx_append_ha(NEWCONST char *sense, const char *stuff, struct ast_ha *path, int *error);
struct ast_context *pbx_context_find_or_create(struct ast_context **extcontexts, struct ast_hashtab *exttable, const char *name, const char *registrar);
struct ast_config *pbx_config_load(const char *filename, const char *who_asked, struct ast_flags flags);
PBX_VARIABLE_TYPE *pbx_variable_new(PBX_VARIABLE_TYPE *v);
const char *pbx_inet_ntoa(struct in_addr ia);
int pbx_str2cos(const char *value, unsigned int *cos);
int pbx_str2tos(const char *value, unsigned int *tos);
int pbx_context_remove_extension(const char *context, const char *extension, int priority, const char *registrar);
void pbxman_send_listack(struct mansession *s, const struct message *m, char *msg, char *listflag);
int pbx_moh_start(PBX_CHANNEL_TYPE * chan, const char *mclass, const char *interpclass);
PBX_CHANNEL_TYPE * sccp_search_remotepeer_locked( int(*const found_cb)(PBX_CHANNEL_TYPE *c, void *data), void *data);
const char *pbx_inet_ntoa(struct in_addr ia);

//#if ASTERISK_VERSION_NUMBER >=10900
//#define ast_format_type struct ast_format
//#else
#    define ast_format_type int
//#endif

skinny_codec_t pbx_codec2skinny_codec(ast_format_type fmt);
ast_format_type skinny_codec2pbx_codec(skinny_codec_t codec);

// support for old uin32_t format (only temporarily
#    define pbx_format2skinny_format (uint32_t)pbx_codec2skinny_codec
#    define skinny_format2pbx_format(_x) skinny_codec2pbx_codec((skinny_codec_t)_x)

//skinny_codec_t *pbx_codecs2skinny_codecs(int codecs);
int skinny_codecs2pbx_codecs(skinny_codec_t * skinny_codecs);

//skinny_codec_t *pbx_codec_pref2sccp_codec_pref(struct ast_codec_pref *prefs);
//struct ast_codec_pref *sccp_codec_pref2pbx_codec_pref(skinny_codec_t *skinny_codecs);

#    define sccp_strndup(str, len) \
	_sccp_strndup((str), (len), __FILE__, __LINE__, __PRETTY_FUNCTION__)

	extern inline char *_sccp_strndup(const char *str, size_t len, const char *file, int lineno, const char *func);

	extern inline char *_sccp_strndup(const char *str, size_t len, const char *file, int lineno, const char *func) {
		char *newstr = NULL;

		if (str) {
                        if (!(newstr = strndup(str, len)))
				ast_log(LOG_ERROR, "Memory Allocation Failure in function %s at line %d of %s\n", func, lineno, file);
      		}
                return newstr;
	}

/* 
 * sccp_free_ptr should be used when a function pointer for free() needs to be 
 * passed as the argument to a function. Otherwise, astmm will cause seg faults.
 */
   	static void sccp_free_ptr(void *ptr) attribute_unused;
	static void sccp_free_ptr(void *ptr) {
	        sccp_free(ptr);
	}

#endif