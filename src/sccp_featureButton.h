/*!
 * \file        sccp_featureButton.h
 * \brief       SCCP FeatureButton Header
 * \author      Marcello Ceschia <marcello [at] ceschia.de>
 * \note        This program is free software and may be modified and distributed under the terms of the GNU Public License.
 *              See the LICENSE file at the top of the source tree.
 * \since       2009-06-15
 *
 * $Date$
 * $Revision$  
 */

#ifndef SCCP_FEATUREBUTTON_H_
#define SCCP_FEATUREBUTTON_H_

void sccp_featButton_changed(sccp_device_t * device, sccp_feature_type_t featureType);

#ifdef CS_DEVSTATE_FEATURE
void sccp_devstateFeatureState_cb(const struct ast_event *ast_event, void *data);
#endif

#endif														/* SCCP_FEATUREBUTTON_H_ */
// kate: indent-width 8; replace-tabs off; indent-mode cstyle; auto-insert-doxygen on; line-numbers on; tab-indents on; keep-extra-spaces off; auto-brackets off;
