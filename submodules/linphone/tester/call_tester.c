/*
	belle-sip - SIP (RFC3261) library.
    Copyright (C) 2010  Belledonne Communications SARL

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include "CUnit/Basic.h"
#include "linphonecore.h"
#include "lpconfig.h"
#include "private.h"
#include "liblinphone_tester.h"


void call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *msg){
	char* to=linphone_address_as_string(linphone_call_get_call_log(call)->to);
	char* from=linphone_address_as_string(linphone_call_get_call_log(call)->from);
	stats* counters;
	ms_message(" %s call from [%s] to [%s], new state is [%s]"	,linphone_call_get_call_log(call)->dir==LinphoneCallIncoming?"Incoming":"Outgoing"
																,from
																,to
																,linphone_call_state_to_string(cstate));
	ms_free(to);
	ms_free(from);
	counters = get_stats(lc);
	switch (cstate) {
	case LinphoneCallIncomingReceived:counters->number_of_LinphoneCallIncomingReceived++;break;
	case LinphoneCallOutgoingInit :counters->number_of_LinphoneCallOutgoingInit++;break;
	case LinphoneCallOutgoingProgress :counters->number_of_LinphoneCallOutgoingProgress++;break;
	case LinphoneCallOutgoingRinging :counters->number_of_LinphoneCallOutgoingRinging++;break;
	case LinphoneCallOutgoingEarlyMedia :counters->number_of_LinphoneCallOutgoingEarlyMedia++;break;
	case LinphoneCallConnected :counters->number_of_LinphoneCallConnected++;break;
	case LinphoneCallStreamsRunning :counters->number_of_LinphoneCallStreamsRunning++;break;
	case LinphoneCallPausing :counters->number_of_LinphoneCallPausing++;break;
	case LinphoneCallPaused :counters->number_of_LinphoneCallPaused++;break;
	case LinphoneCallResuming :counters->number_of_LinphoneCallResuming++;break;
	case LinphoneCallRefered :counters->number_of_LinphoneCallRefered++;break;
	case LinphoneCallError :counters->number_of_LinphoneCallError++;break;
	case LinphoneCallEnd :counters->number_of_LinphoneCallEnd++;break;
	case LinphoneCallPausedByRemote :counters->number_of_LinphoneCallPausedByRemote++;break;
	case LinphoneCallUpdatedByRemote :counters->number_of_LinphoneCallUpdatedByRemote++;break;
	case LinphoneCallIncomingEarlyMedia :counters->number_of_LinphoneCallIncomingEarlyMedia++;break;
	case LinphoneCallUpdating :counters->number_of_LinphoneCallUpdating++;break;
	case LinphoneCallReleased :counters->number_of_LinphoneCallReleased++;break;
	default:
		CU_FAIL("unexpected event");break;
	}
}

void linphone_transfer_state_changed(LinphoneCore *lc, LinphoneCall *transfered, LinphoneCallState new_call_state) {
	char* to=linphone_address_as_string(linphone_call_get_call_log(transfered)->to);
	char* from=linphone_address_as_string(linphone_call_get_call_log(transfered)->from);
	stats* counters;
	ms_message("Transferred call from [%s] to [%s], new state is [%s]",from,to,linphone_call_state_to_string(new_call_state));
	ms_free(to);
	ms_free(from);

	counters = get_stats(lc);
	switch (new_call_state) {
	case LinphoneCallOutgoingInit :counters->number_of_LinphoneTransferCallOutgoingInit++;break;
	case LinphoneCallOutgoingProgress :counters->number_of_LinphoneTransferCallOutgoingProgress++;break;
	case LinphoneCallOutgoingRinging :counters->number_of_LinphoneTransferCallOutgoingRinging++;break;
	case LinphoneCallOutgoingEarlyMedia :counters->number_of_LinphoneTransferCallOutgoingEarlyMedia++;break;
	case LinphoneCallConnected :counters->number_of_LinphoneTransferCallConnected++;break;
	case LinphoneCallStreamsRunning :counters->number_of_LinphoneTransferCallStreamsRunning++;break;
	case LinphoneCallError :counters->number_of_LinphoneTransferCallError++;break;
	default:
		CU_FAIL("unexpected event");break;
	}
}

#ifdef VIDEO_ENABLED
static void linphone_call_cb(LinphoneCall *call,void * user_data) {
	char* to=linphone_address_as_string(linphone_call_get_call_log(call)->to);
	char* from=linphone_address_as_string(linphone_call_get_call_log(call)->from);
	stats* counters;
	LinphoneCore* lc=(LinphoneCore*)user_data;
	ms_message("call from [%s] to [%s] receive iFrame",from,to);
	ms_free(to);
	ms_free(from);
	counters = (stats*)get_stats(lc);
	counters->number_of_IframeDecoded++;
}
#endif

bool_t call_with_params(LinphoneCoreManager* caller_mgr
						,LinphoneCoreManager* callee_mgr
						, const LinphoneCallParams *caller_params
						, const LinphoneCallParams *callee_params) {
	int retry=0;
	stats initial_caller=caller_mgr->stat;
	stats initial_callee=callee_mgr->stat;
	bool_t result=FALSE;

	if (!caller_params){
		CU_ASSERT_PTR_NOT_NULL(linphone_core_invite_address(caller_mgr->lc,callee_mgr->identity));
	}else{
		CU_ASSERT_PTR_NOT_NULL(linphone_core_invite_address_with_params(caller_mgr->lc,callee_mgr->identity,caller_params));
	}

	/*linphone_core_invite(caller_mgr->lc,"pauline");*/

	CU_ASSERT_TRUE(wait_for(callee_mgr->lc
									,caller_mgr->lc
									,&callee_mgr->stat.number_of_LinphoneCallIncomingReceived
									,initial_callee.number_of_LinphoneCallIncomingReceived+1));
	CU_ASSERT_TRUE(linphone_core_inc_invite_pending(callee_mgr->lc));
	CU_ASSERT_EQUAL(caller_mgr->stat.number_of_LinphoneCallOutgoingProgress,initial_caller.number_of_LinphoneCallOutgoingProgress+1);


	while (caller_mgr->stat.number_of_LinphoneCallOutgoingRinging!=(initial_caller.number_of_LinphoneCallOutgoingRinging + 1)
			&& caller_mgr->stat.number_of_LinphoneCallOutgoingEarlyMedia!=(initial_caller.number_of_LinphoneCallOutgoingEarlyMedia +1)
			&& retry++ <20) {
			linphone_core_iterate(caller_mgr->lc);
			linphone_core_iterate(callee_mgr->lc);
			ms_usleep(100000);
	}


	CU_ASSERT_TRUE((caller_mgr->stat.number_of_LinphoneCallOutgoingRinging==initial_caller.number_of_LinphoneCallOutgoingRinging+1)
							|(caller_mgr->stat.number_of_LinphoneCallOutgoingEarlyMedia==initial_caller.number_of_LinphoneCallOutgoingEarlyMedia+1));


	CU_ASSERT_PTR_NOT_NULL(linphone_core_get_current_call_remote_address(callee_mgr->lc));
	if (!linphone_core_get_current_call_remote_address(callee_mgr->lc))
		return 0;
	if (linphone_call_params_get_privacy(linphone_call_get_current_params(linphone_core_get_current_call(caller_mgr->lc))) == LinphonePrivacyNone) {
		CU_ASSERT_TRUE(linphone_address_weak_equal(caller_mgr->identity,linphone_core_get_current_call_remote_address(callee_mgr->lc)));
	} else {
		CU_ASSERT_FALSE(linphone_address_weak_equal(caller_mgr->identity,linphone_core_get_current_call_remote_address(callee_mgr->lc)));
	}

	if (callee_params)
		linphone_core_accept_call_with_params(callee_mgr->lc,linphone_core_get_current_call(callee_mgr->lc),callee_params);
	else
		linphone_core_accept_call(callee_mgr->lc,linphone_core_get_current_call(callee_mgr->lc));

	CU_ASSERT_TRUE(wait_for(callee_mgr->lc,caller_mgr->lc,&callee_mgr->stat.number_of_LinphoneCallConnected,initial_callee.number_of_LinphoneCallConnected+1));
	CU_ASSERT_TRUE(wait_for(callee_mgr->lc,caller_mgr->lc,&caller_mgr->stat.number_of_LinphoneCallConnected,initial_callee.number_of_LinphoneCallConnected+1));
	/*just to sleep*/
	result = wait_for(callee_mgr->lc,caller_mgr->lc,&caller_mgr->stat.number_of_LinphoneCallStreamsRunning,initial_caller.number_of_LinphoneCallStreamsRunning+1)
			&&
			wait_for(callee_mgr->lc,caller_mgr->lc,&callee_mgr->stat.number_of_LinphoneCallStreamsRunning,initial_callee.number_of_LinphoneCallStreamsRunning+1);

	if (linphone_core_get_media_encryption(caller_mgr->lc)
		&& linphone_core_get_media_encryption(callee_mgr->lc)) {
		const LinphoneCallParams* call_param = linphone_call_get_current_params(linphone_core_get_current_call(callee_mgr->lc));
		CU_ASSERT_EQUAL(linphone_call_params_get_media_encryption(call_param),linphone_core_get_media_encryption(caller_mgr->lc));
		call_param = linphone_call_get_current_params(linphone_core_get_current_call(caller_mgr->lc));
		CU_ASSERT_EQUAL(linphone_call_params_get_media_encryption(call_param),linphone_core_get_media_encryption(caller_mgr->lc));
	}
	return result;
}
bool_t call_with_caller_params(LinphoneCoreManager* caller_mgr,LinphoneCoreManager* callee_mgr, const LinphoneCallParams *params) {
	return call_with_params(caller_mgr,callee_mgr,params,NULL);
}
bool_t call(LinphoneCoreManager* caller_mgr,LinphoneCoreManager* callee_mgr){
	return call_with_params(caller_mgr,callee_mgr,NULL,NULL);
}

static void simple_call(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");

	LinphoneCore* lc_marie=marie->lc;
	LinphoneCore* lc_pauline=pauline->lc;
	stats* stat_marie=&marie->stat;
	stats* stat_pauline=&pauline->stat;
	LinphoneProxyConfig* proxy;
	LinphoneAddress* identity;


	linphone_core_invite(lc_marie,"pauline");

	CU_ASSERT_TRUE (wait_for(lc_pauline,lc_marie,&stat_pauline->number_of_LinphoneCallIncomingReceived,1));
	CU_ASSERT_TRUE(linphone_core_inc_invite_pending(lc_pauline));
	CU_ASSERT_EQUAL(stat_marie->number_of_LinphoneCallOutgoingProgress,1);
	CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_marie->number_of_LinphoneCallOutgoingRinging,1));

	linphone_core_get_default_proxy(lc_marie,&proxy);
	CU_ASSERT_PTR_NOT_NULL (proxy);
	identity = linphone_address_new(linphone_proxy_config_get_identity(proxy));
	CU_ASSERT_PTR_NOT_NULL(linphone_core_get_current_call_remote_address(lc_pauline));
	if (linphone_core_get_current_call_remote_address(lc_pauline)) {
		CU_ASSERT_TRUE(linphone_address_weak_equal(identity,linphone_core_get_current_call_remote_address(lc_pauline)));
		linphone_address_destroy(identity);

		linphone_core_accept_call(lc_pauline,linphone_core_get_current_call(lc_pauline));

		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_pauline->number_of_LinphoneCallConnected,1));
		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_marie->number_of_LinphoneCallConnected,1));
		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_pauline->number_of_LinphoneCallStreamsRunning,1));
		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_marie->number_of_LinphoneCallStreamsRunning,1));
		/*just to sleep*/
		wait_for(lc_pauline,lc_marie,&stat_marie->number_of_LinphoneCallStreamsRunning,3);
		linphone_core_terminate_all_calls(lc_pauline);
		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_pauline->number_of_LinphoneCallEnd,1));
		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_marie->number_of_LinphoneCallEnd,1));
	}
	linphone_core_destroy(marie->lc);
	marie->lc=NULL;
	CU_ASSERT_EQUAL(stat_marie->number_of_LinphoneCallReleased,1);
	linphone_core_destroy(pauline->lc);
	pauline->lc=NULL;
	CU_ASSERT_EQUAL(stat_pauline->number_of_LinphoneCallReleased,1);

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void simple_call_compatibility_mode(void) {
	char route[256];
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");

	LinphoneCore* lc_marie=marie->lc;
	LinphoneCore* lc_pauline=pauline->lc;
	stats* stat_marie=&marie->stat;
	stats* stat_pauline=&pauline->stat;
	LinphoneProxyConfig* proxy;
	LinphoneAddress* identity;
	LinphoneAddress* proxy_address;
	char*tmp;
	LCSipTransports transport;

	linphone_core_get_default_proxy(lc_marie,&proxy);
	CU_ASSERT_PTR_NOT_NULL (proxy);
	identity = linphone_address_new(linphone_proxy_config_get_identity(proxy));


	proxy_address=linphone_address_new(linphone_proxy_config_get_addr(proxy));
	linphone_address_clean(proxy_address);
	tmp=linphone_address_as_string_uri_only(proxy_address);
	linphone_proxy_config_set_server_addr(proxy,tmp);
	sprintf(route,"sip:%s",test_route);
	linphone_proxy_config_set_route(proxy,route);
	ms_free(tmp);
	linphone_address_destroy(proxy_address);
	linphone_core_get_sip_transports(lc_marie,&transport);
	transport.udp_port=0;
	transport.tls_port=0;
	transport.dtls_port=0;
	/*only keep tcp*/
	linphone_core_set_sip_transports(lc_marie,&transport);
	stat_marie->number_of_LinphoneRegistrationOk=0;

	CU_ASSERT_TRUE (wait_for(lc_marie,lc_marie,&stat_marie->number_of_LinphoneRegistrationOk,1));

	linphone_core_invite(lc_marie,"pauline");

	CU_ASSERT_TRUE (wait_for(lc_pauline,lc_marie,&stat_pauline->number_of_LinphoneCallIncomingReceived,1));
	CU_ASSERT_TRUE(linphone_core_inc_invite_pending(lc_pauline));
	CU_ASSERT_EQUAL(stat_marie->number_of_LinphoneCallOutgoingProgress,1);
	CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_marie->number_of_LinphoneCallOutgoingRinging,1));

	CU_ASSERT_PTR_NOT_NULL(linphone_core_get_current_call_remote_address(lc_pauline));
	if (linphone_core_get_current_call_remote_address(lc_pauline)) {
		CU_ASSERT_TRUE(linphone_address_weak_equal(identity,linphone_core_get_current_call_remote_address(lc_pauline)));
		linphone_address_destroy(identity);

		linphone_core_accept_call(lc_pauline,linphone_core_get_current_call(lc_pauline));

		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_pauline->number_of_LinphoneCallConnected,1));
		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_marie->number_of_LinphoneCallConnected,1));
		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_pauline->number_of_LinphoneCallStreamsRunning,1));
		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_marie->number_of_LinphoneCallStreamsRunning,1));
		/*just to sleep*/
		wait_for(lc_pauline,lc_marie,&stat_marie->number_of_LinphoneCallStreamsRunning,3);
		linphone_core_terminate_all_calls(lc_pauline);
		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_pauline->number_of_LinphoneCallEnd,1));
		CU_ASSERT_TRUE(wait_for(lc_pauline,lc_marie,&stat_marie->number_of_LinphoneCallEnd,1));
	}
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}


static void cancelled_call(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");

	LinphoneCall* out_call = linphone_core_invite(pauline->lc,"marie");
	linphone_call_ref(out_call);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallOutgoingInit,1));

	linphone_core_terminate_call(pauline->lc,out_call);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	//CU_ASSERT_EQUAL(linphone_call_get_reason(out_call),LinphoneReasonCanceled);
	CU_ASSERT_EQUAL(pauline->stat.number_of_LinphoneCallEnd,1);
	CU_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallIncomingReceived,0);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallReleased,1));

	linphone_call_unref(out_call);
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void call_with_dns_time_out(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new2( "empty_rc", FALSE);
	LCSipTransports transport = {9773,0,0,0};
	linphone_core_set_sip_transports(marie->lc,&transport);
	linphone_core_iterate(marie->lc);
	sal_set_dns_timeout(marie->lc->sal,0);
	linphone_core_invite(marie->lc,"sip:toto@toto.com");
	linphone_core_iterate(marie->lc);
	linphone_core_iterate(marie->lc);
	CU_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallOutgoingInit,1);
	CU_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallOutgoingProgress,1);
	CU_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallError,1);
	CU_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallReleased,1);
	linphone_core_manager_destroy(marie);
}

static void early_cancelled_call(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_alt_rc");

	LinphoneCall* out_call = linphone_core_invite(pauline->lc,"sip:marie@sip.example.org");
	
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallOutgoingInit,1));
	linphone_core_terminate_call(pauline->lc,out_call);
	
	/*since everything is executed in a row, no response can be received from the server, thus the CANCEL cannot be sent.
	 It will ring at Marie's side.*/
	
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	
	CU_ASSERT_EQUAL(pauline->stat.number_of_LinphoneCallEnd,1);
	
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallIncomingReceived,1));
	/* now the CANCEL should have been sent and the the call at marie's side should terminate*/
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));
	
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallReleased,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void cancelled_ringing_call(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");

	LinphoneCall* out_call = linphone_core_invite(pauline->lc,"marie");
	linphone_call_ref(out_call);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallIncomingReceived,1));

	linphone_core_terminate_call(pauline->lc,out_call);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallReleased,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallReleased,1));
	CU_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallEnd,1);
	CU_ASSERT_EQUAL(pauline->stat.number_of_LinphoneCallEnd,1);

	linphone_call_unref(out_call);
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void early_declined_call(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCallLog* out_call_log;
	LinphoneCall* out_call;

	linphone_core_set_max_calls(marie->lc,0);
	out_call = linphone_core_invite(pauline->lc,"marie");
	linphone_call_ref(out_call);

	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallError,1));
	CU_ASSERT_EQUAL(pauline->stat.number_of_LinphoneCallError,1);
	/* FIXME http://git.linphone.org/mantis/view.php?id=757
	
	CU_ASSERT_EQUAL(linphone_call_get_reason(out_call),LinphoneReasonBusy);
	 */
	if (ms_list_size(linphone_core_get_call_logs(pauline->lc))>0) {
		CU_ASSERT_PTR_NOT_NULL(out_call_log=(LinphoneCallLog*)(linphone_core_get_call_logs(pauline->lc)->data));
		CU_ASSERT_EQUAL(linphone_call_log_get_status(out_call_log),LinphoneCallAborted);
	}
	linphone_call_unref(out_call);
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void call_declined(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");

	LinphoneCall* in_call;
	LinphoneCall* out_call = linphone_core_invite(pauline->lc,"marie");
	linphone_call_ref(out_call);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallIncomingReceived,1));
	CU_ASSERT_PTR_NOT_NULL(in_call=linphone_core_get_current_call(marie->lc));
	if (in_call) {
		linphone_call_ref(in_call);
		linphone_core_terminate_call(marie->lc,in_call);
		CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallReleased,1));
		CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallReleased,1));
		CU_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallEnd,1);
		CU_ASSERT_EQUAL(pauline->stat.number_of_LinphoneCallEnd,1);
		CU_ASSERT_EQUAL(linphone_call_get_reason(in_call),LinphoneReasonDeclined);
		CU_ASSERT_EQUAL(linphone_call_get_reason(out_call),LinphoneReasonDeclined);
		linphone_call_unref(in_call);
	}
	linphone_call_unref(out_call);
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void call_terminated_by_caller(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	
	CU_ASSERT_TRUE(call(pauline,marie));
	/*just to sleep*/
	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void call_with_no_sdp(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_no_sdp_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	
	CU_ASSERT_TRUE(call(marie,pauline));
	/*just to sleep*/
	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static bool_t check_ice(LinphoneCoreManager* caller, LinphoneCoreManager* callee, LinphoneIceState state) {
	LinphoneCall *c1,*c2;
	bool_t success=FALSE;
	int i;

	c1=linphone_core_get_current_call(caller->lc);
	c2=linphone_core_get_current_call(callee->lc);
	
	CU_ASSERT_PTR_NOT_NULL(c1);
	CU_ASSERT_PTR_NOT_NULL(c2);

	for (i=0;i<200;i++){
		if ((c1 != NULL) && (c2 != NULL)) {
			if (linphone_call_get_audio_stats(c1)->ice_state==LinphoneIceStateHostConnection &&
				linphone_call_get_audio_stats(c2)->ice_state==LinphoneIceStateHostConnection ){
				success=TRUE;
				break;
			}
			linphone_core_iterate(caller->lc);
			linphone_core_iterate(callee->lc);
		}
		ms_usleep(50000);
	}
	return success;
}
static void call_with_ice(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	
	linphone_core_set_firewall_policy(marie->lc,LinphonePolicyUseIce);
	linphone_core_set_stun_server(marie->lc,"stun.linphone.org");
	linphone_core_set_firewall_policy(pauline->lc,LinphonePolicyUseIce);
	linphone_core_set_stun_server(pauline->lc,"stun.linphone.org");

	CU_ASSERT_TRUE(call(pauline,marie));

	CU_ASSERT_TRUE(check_ice(pauline,marie,LinphoneIceStateHostConnection));
	/*wait for the ICE reINVITE to complete*/
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallStreamsRunning,2));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallStreamsRunning,2));
	
	/*then close the call*/
	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void call_with_custom_headers(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCall *c1,*c2;
	LinphoneCallParams *params;
	const LinphoneCallParams *remote_params;
	const char *hvalue;
	
	params=linphone_core_create_default_call_parameters(marie->lc);
	linphone_call_params_add_custom_header(params,"Weather","bad");
	linphone_call_params_add_custom_header(params,"Working","yes");
	
	CU_ASSERT_TRUE(call_with_caller_params(pauline,marie,params));
	linphone_call_params_destroy(params);
	
	c1=linphone_core_get_current_call(marie->lc);
	c2=linphone_core_get_current_call(pauline->lc);
	
	CU_ASSERT_PTR_NOT_NULL(c1);
	CU_ASSERT_PTR_NOT_NULL(c2);

	remote_params=linphone_call_get_remote_params(c1);
	hvalue=linphone_call_params_get_custom_header(remote_params,"Weather");
	CU_ASSERT_PTR_NOT_NULL(hvalue);
	CU_ASSERT_TRUE(strcmp(hvalue,"bad")==0);
	
	CU_ASSERT_PTR_NOT_NULL(linphone_call_get_remote_contact(c1));
	
	/*just to sleep*/
	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void call_paused_resumed(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCall* call_obj;

	CU_ASSERT_TRUE(call(pauline,marie));
	call_obj = linphone_core_get_current_call(pauline->lc);

	linphone_core_pause_call(pauline->lc,call_obj);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallPausing,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallPausedByRemote,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallPaused,1));

	linphone_core_resume_call(pauline->lc,call_obj);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallStreamsRunning,2));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallStreamsRunning,2));

	/*just to sleep*/
	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static bool_t pause_call_1(LinphoneCoreManager* mgr_1,LinphoneCall* call_1,LinphoneCoreManager* mgr_2,LinphoneCall* call_2) {
	stats initial_call_stat_1=mgr_1->stat;
	stats initial_call_stat_2=mgr_2->stat;
	linphone_core_pause_call(mgr_1->lc,call_1);
	CU_ASSERT_TRUE(wait_for(mgr_1->lc,mgr_2->lc,&mgr_1->stat.number_of_LinphoneCallPausing,initial_call_stat_1.number_of_LinphoneCallPausing+1));
	CU_ASSERT_TRUE(wait_for(mgr_1->lc,mgr_2->lc,&mgr_1->stat.number_of_LinphoneCallPaused,initial_call_stat_1.number_of_LinphoneCallPaused+1));
	CU_ASSERT_TRUE(wait_for(mgr_1->lc,mgr_2->lc,&mgr_2->stat.number_of_LinphoneCallPausedByRemote,initial_call_stat_2.number_of_LinphoneCallPausedByRemote+1));
	CU_ASSERT_EQUAL(linphone_call_get_state(call_1),LinphoneCallPaused);
	CU_ASSERT_EQUAL(linphone_call_get_state(call_2),LinphoneCallPausedByRemote);
	return linphone_call_get_state(call_1) == LinphoneCallPaused && linphone_call_get_state(call_2)==LinphoneCallPausedByRemote;
}

static void call_paused_resumed_from_callee(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCall* call_obj;

	CU_ASSERT_TRUE(call(pauline,marie));
	call_obj = linphone_core_get_current_call(marie->lc);

	linphone_core_pause_call(marie->lc,call_obj);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallPausing,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallPausedByRemote,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallPaused,1));

	linphone_core_resume_call(marie->lc,call_obj);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallStreamsRunning,2));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallStreamsRunning,2));

	/*just to sleep*/
	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

#ifdef VIDEO_ENABLED
static bool_t add_video(LinphoneCoreManager* caller,LinphoneCoreManager* callee) {
	LinphoneVideoPolicy  caller_policy;
	LinphoneCallParams* callee_params;
	LinphoneCall* call_obj;
	caller_policy.automatically_accept=TRUE;
	caller_policy.automatically_initiate=TRUE;
	linphone_core_enable_video(callee->lc,TRUE,TRUE);
	linphone_core_enable_video(caller->lc,TRUE,FALSE);
	linphone_core_set_video_policy(caller->lc,&caller_policy);
	stats initial_caller_stat=caller->stat;
	stats initial_callee_stat=callee->stat;

	call_obj = linphone_core_get_current_call(callee->lc);
	callee_params = linphone_call_params_copy(linphone_call_get_current_params(call_obj));
	/*add video*/
	linphone_call_params_enable_video(callee_params,TRUE);
	linphone_core_update_call(callee->lc,call_obj,callee_params);

	CU_ASSERT_TRUE(wait_for(caller->lc,callee->lc,&caller->stat.number_of_LinphoneCallUpdatedByRemote,initial_caller_stat.number_of_LinphoneCallUpdatedByRemote+1));
	CU_ASSERT_TRUE(wait_for(caller->lc,callee->lc,&callee->stat.number_of_LinphoneCallUpdating,initial_callee_stat.number_of_LinphoneCallUpdating+1));
	CU_ASSERT_TRUE(wait_for(caller->lc,callee->lc,&callee->stat.number_of_LinphoneCallStreamsRunning,initial_callee_stat.number_of_LinphoneCallStreamsRunning+1));
	CU_ASSERT_TRUE(wait_for(caller->lc,callee->lc,&caller->stat.number_of_LinphoneCallStreamsRunning,initial_caller_stat.number_of_LinphoneCallStreamsRunning+1));

	CU_ASSERT_TRUE(linphone_call_params_video_enabled(linphone_call_get_current_params(linphone_core_get_current_call(callee->lc))));
	CU_ASSERT_TRUE(linphone_call_params_video_enabled(linphone_call_get_current_params(linphone_core_get_current_call(caller->lc))));

	linphone_call_set_next_video_frame_decoded_callback(call_obj,linphone_call_cb,callee->lc);
	/*send vfu*/
	linphone_call_send_vfu_request(call_obj);
	return wait_for(caller->lc,callee->lc,&callee->stat.number_of_IframeDecoded,initial_callee_stat.number_of_IframeDecoded+1);

}
static void call_with_video_added(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	CU_ASSERT_TRUE(call(pauline,marie));

	CU_ASSERT_TRUE(add_video(pauline,marie));
	/*just to sleep*/
	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void call_with_declined_video(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCallParams* callee_params;
	LinphoneCallParams* caller_params;
	LinphoneCall* marie_call;
	LinphoneCall* pauline_call;

	linphone_core_enable_video(marie->lc,TRUE,TRUE);
	linphone_core_enable_video(pauline->lc,TRUE,FALSE);

	caller_params=linphone_core_create_default_call_parameters(marie->lc);
	linphone_call_params_enable_video(caller_params,TRUE);
	callee_params=linphone_core_create_default_call_parameters(pauline->lc);
	linphone_call_params_enable_video(callee_params,FALSE);
	CU_ASSERT_TRUE(call_with_params(pauline,marie,caller_params,callee_params));
	marie_call=linphone_core_get_current_call(marie->lc);
	pauline_call=linphone_core_get_current_call(pauline->lc);

	CU_ASSERT_FALSE(linphone_call_log_video_enabled(linphone_call_get_call_log(marie_call)));
	CU_ASSERT_FALSE(linphone_call_log_video_enabled(linphone_call_get_call_log(pauline_call)));


	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void video_call(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCallParams* callee_params;
	LinphoneCallParams* caller_params;
	LinphoneCall* marie_call;
	LinphoneCall* pauline_call;

	linphone_core_enable_video(marie->lc,TRUE,TRUE);
	linphone_core_enable_video(pauline->lc,TRUE,FALSE);

	caller_params=linphone_core_create_default_call_parameters(marie->lc);
	linphone_call_params_enable_video(caller_params,TRUE);
	callee_params=linphone_core_create_default_call_parameters(pauline->lc);
	linphone_call_params_enable_video(callee_params,TRUE);
	CU_ASSERT_TRUE(call_with_params(pauline,marie,caller_params,callee_params));
	marie_call=linphone_core_get_current_call(marie->lc);
	pauline_call=linphone_core_get_current_call(pauline->lc);

	CU_ASSERT_TRUE(linphone_call_log_video_enabled(linphone_call_get_call_log(marie_call)));
	CU_ASSERT_TRUE(linphone_call_log_video_enabled(linphone_call_get_call_log(pauline_call)));

	/*check video path*/
	linphone_call_set_next_video_frame_decoded_callback(marie_call,linphone_call_cb,marie->lc);
	linphone_call_send_vfu_request(marie_call);
	CU_ASSERT_TRUE( wait_for(marie->lc,pauline->lc,&marie->stat.number_of_IframeDecoded,1));

	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}
#endif

static void call_with_privacy(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCall *c1,*c2;
	LinphoneCallParams *params;
	LinphoneProxyConfig* pauline_proxy;
	params=linphone_core_create_default_call_parameters(pauline->lc);
	linphone_call_params_set_privacy(params,LinphonePrivacyId);

	CU_ASSERT_TRUE(call_with_caller_params(pauline,marie,params));
	linphone_call_params_destroy(params);

	c1=linphone_core_get_current_call(pauline->lc);
	c2=linphone_core_get_current_call(marie->lc);

	CU_ASSERT_PTR_NOT_NULL(c1);
	CU_ASSERT_PTR_NOT_NULL(c2);

	/*make sure local identity is unchanged*/
	CU_ASSERT_TRUE(linphone_address_weak_equal(linphone_call_log_get_from(linphone_call_get_call_log(c1)),pauline->identity));

	/*make sure remote identity is hidden*/
	CU_ASSERT_FALSE(linphone_address_weak_equal(linphone_call_get_remote_address(c2),pauline->identity));

	CU_ASSERT_EQUAL(linphone_call_params_get_privacy(linphone_call_get_current_params(c2)),LinphonePrivacyId);

	/*just to sleep*/
	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	/*test proxy config privacy*/
	linphone_core_get_default_proxy(pauline->lc,&pauline_proxy);
	linphone_proxy_config_set_privacy(pauline_proxy,LinphonePrivacyId);

	CU_ASSERT_TRUE(call(pauline,marie));
	c1=linphone_core_get_current_call(pauline->lc);
	c2=linphone_core_get_current_call(marie->lc);

	CU_ASSERT_PTR_NOT_NULL(c1);
	CU_ASSERT_PTR_NOT_NULL(c2);

	/*make sure remote identity is hidden*/
	CU_ASSERT_FALSE(linphone_address_weak_equal(linphone_call_get_remote_address(c2),pauline->identity));

	CU_ASSERT_EQUAL(linphone_call_params_get_privacy(linphone_call_get_current_params(c2)),LinphonePrivacyId);
	/*just to sleep*/
	linphone_core_terminate_all_calls(pauline->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,2));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,2));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}




static void simple_conference(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCoreManager* laure = linphone_core_manager_new( "laure_rc");
	stats initial_marie_stat;
	stats initial_pauline_stat;
	stats initial_laure_stat;

	LinphoneCall* marie_call_pauline;
	LinphoneCall* pauline_called_by_marie;
	LinphoneCall* marie_call_laure;

	MSList* lcs=ms_list_append(NULL,marie->lc);
	lcs=ms_list_append(lcs,pauline->lc);
	lcs=ms_list_append(lcs,laure->lc);

	CU_ASSERT_TRUE(call(marie,pauline));
	marie_call_pauline=linphone_core_get_current_call(marie->lc);
	pauline_called_by_marie=linphone_core_get_current_call(pauline->lc);
	CU_ASSERT_TRUE(pause_call_1(marie,marie_call_pauline,pauline,pauline_called_by_marie));

	CU_ASSERT_TRUE(call(marie,laure));
	initial_marie_stat=marie->stat;
	initial_pauline_stat=pauline->stat;
	initial_laure_stat=laure->stat;

	marie_call_laure=linphone_core_get_current_call(marie->lc);

	linphone_core_add_to_conference(marie->lc,marie_call_laure);
	CU_ASSERT_TRUE(wait_for(marie->lc,laure->lc,&marie->stat.number_of_LinphoneCallUpdating,initial_marie_stat.number_of_LinphoneCallUpdating+1));



	linphone_core_add_to_conference(marie->lc,marie_call_pauline);

	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallResuming,initial_marie_stat.number_of_LinphoneCallResuming+1,2000));

	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallStreamsRunning,initial_pauline_stat.number_of_LinphoneCallStreamsRunning+1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallStreamsRunning,initial_laure_stat.number_of_LinphoneCallStreamsRunning+1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallStreamsRunning,initial_marie_stat.number_of_LinphoneCallStreamsRunning+2,3000));

	CU_ASSERT_TRUE(linphone_core_is_in_conference(marie->lc));
	CU_ASSERT_EQUAL(linphone_core_get_conference_size(marie->lc),3)

	linphone_core_terminate_conference(marie->lc);

	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallEnd,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallEnd,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallEnd,1,2000));


	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
	linphone_core_manager_destroy(laure);
	ms_list_free(lcs);
}


static void srtp_call(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");

	if (linphone_core_media_encryption_supported(marie->lc,LinphoneMediaEncryptionSRTP)) {
		linphone_core_set_media_encryption(marie->lc,LinphoneMediaEncryptionSRTP);
		linphone_core_set_media_encryption(pauline->lc,LinphoneMediaEncryptionSRTP);

		CU_ASSERT_TRUE(call(pauline,marie));

		CU_ASSERT_EQUAL(linphone_core_get_media_encryption(marie->lc),LinphoneMediaEncryptionSRTP);
		CU_ASSERT_EQUAL(linphone_core_get_media_encryption(pauline->lc),LinphoneMediaEncryptionSRTP);

		/*just to sleep*/
		linphone_core_terminate_all_calls(marie->lc);
		CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
		CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));
	} else {
		ms_warning ("not tested because srtp not available");
	}
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}
static void call_with_declined_srtp(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	if (linphone_core_media_encryption_supported(marie->lc,LinphoneMediaEncryptionSRTP)) {
		linphone_core_set_media_encryption(pauline->lc,LinphoneMediaEncryptionSRTP);

		CU_ASSERT_TRUE(call(pauline,marie));

		/*just to sleep*/
		linphone_core_terminate_all_calls(marie->lc);
		CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
		CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));
	} else {
		ms_warning ("not tested because srtp not available");
	}
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}
#ifdef VIDEO_ENABLED
static void srtp_video_ice_call(void) {
	int i=0;
#else
static void srtp_ice_call(void) {
#endif
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");

	if (linphone_core_media_encryption_supported(marie->lc,LinphoneMediaEncryptionSRTP)) {
		linphone_core_set_media_encryption(marie->lc,LinphoneMediaEncryptionSRTP);
		linphone_core_set_media_encryption(pauline->lc,LinphoneMediaEncryptionSRTP);

		linphone_core_set_firewall_policy(marie->lc,LinphonePolicyUseIce);
		linphone_core_set_stun_server(marie->lc,"stun.linphone.org");
		linphone_core_set_firewall_policy(pauline->lc,LinphonePolicyUseIce);
		linphone_core_set_stun_server(pauline->lc,"stun.linphone.org");


		CU_ASSERT_TRUE(call(pauline,marie));

		CU_ASSERT_TRUE(check_ice(pauline,marie,LinphoneIceStateHostConnection));
#ifdef VIDEO_ENABLED
		for (i=0;i<100;i++) { /*fixme to workaround a crash*/
			ms_usleep(20000);
			linphone_core_iterate(marie->lc);
			linphone_core_iterate(pauline->lc);
		}

		add_video(pauline,marie);

		CU_ASSERT_TRUE(check_ice(pauline,marie,LinphoneIceStateHostConnection));
#endif
		/*wait for ice to found the direct path*/
		CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_IframeDecoded,1));

		/*just to sleep*/
		linphone_core_terminate_all_calls(marie->lc);
		CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
		CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));
	} else {
		ms_warning ("not tested because srtp not available");
	}
	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}


static void early_media_call(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_early_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");

	CU_ASSERT_TRUE(call(pauline,marie));

	CU_ASSERT_EQUAL(marie->stat.number_of_LinphoneCallIncomingEarlyMedia,1);
	CU_ASSERT_EQUAL(pauline->stat.number_of_LinphoneCallOutgoingEarlyMedia,1);
	/*just to sleep*/
	linphone_core_terminate_all_calls(marie->lc);
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&pauline->stat.number_of_LinphoneCallEnd,1));
	CU_ASSERT_TRUE(wait_for(pauline->lc,marie->lc,&marie->stat.number_of_LinphoneCallEnd,1));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
}

static void simple_call_transfer(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCoreManager* laure = linphone_core_manager_new( "laure_rc");
	LinphoneCall* pauline_called_by_marie;
	LinphoneCall *marie_calling_pauline;
	LinphoneCall *marie_calling_laure;

	char* laure_identity=linphone_address_as_string(laure->identity);
	MSList* lcs=ms_list_append(NULL,marie->lc);
	lcs=ms_list_append(lcs,pauline->lc);
	lcs=ms_list_append(lcs,laure->lc);


	CU_ASSERT_TRUE(call(marie,pauline));
	marie_calling_pauline=linphone_core_get_current_call(marie->lc);
	pauline_called_by_marie=linphone_core_get_current_call(pauline->lc);

	reset_counters(&marie->stat);
	reset_counters(&pauline->stat);
	reset_counters(&laure->stat);


	linphone_core_transfer_call(pauline->lc,pauline_called_by_marie,laure_identity);
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallRefered,1,2000));
	/*marie pausing pauline*/
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallPausing,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallPausedByRemote,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallPaused,1,2000));
	/*marie calling laure*/
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallOutgoingProgress,1,2000));
	
	CU_ASSERT_PTR_NOT_NULL(linphone_call_get_transfer_target_call(marie_calling_pauline));
	
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneTransferCallOutgoingInit,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallIncomingReceived,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallOutgoingRinging,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneTransferCallOutgoingProgress,1,2000));
	linphone_core_accept_call(laure->lc,linphone_core_get_current_call(laure->lc));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallConnected,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallStreamsRunning,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallConnected,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallStreamsRunning,1,2000));
	
	marie_calling_laure=linphone_core_get_current_call(marie->lc);
	CU_ASSERT_PTR_NOT_NULL_FATAL(marie_calling_laure);
	CU_ASSERT_TRUE(linphone_call_get_transferer_call(marie_calling_laure)==marie_calling_pauline);
	
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneTransferCallConnected,1,2000));

	/*terminate marie to pauline call*/
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallEnd,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallEnd,1,2000));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
	linphone_core_manager_destroy(laure);
	ms_list_free(lcs);
}

static void unattended_call_transfer(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCoreManager* laure = linphone_core_manager_new( "laure_rc");
	LinphoneCall* pauline_called_by_marie;

	char* laure_identity=linphone_address_as_string(laure->identity);
	MSList* lcs=ms_list_append(NULL,marie->lc);
	lcs=ms_list_append(lcs,pauline->lc);
	lcs=ms_list_append(lcs,laure->lc);


	CU_ASSERT_TRUE(call(marie,pauline));
	pauline_called_by_marie=linphone_core_get_current_call(marie->lc);

	reset_counters(&marie->stat);
	reset_counters(&pauline->stat);
	reset_counters(&laure->stat);

	linphone_core_transfer_call(marie->lc,pauline_called_by_marie,laure_identity);
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallRefered,1,2000));
	
	/*marie ends the call  */
	linphone_core_terminate_call(marie->lc,pauline_called_by_marie);
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallEnd,1,2000));
	
	/*Pauline starts the transfer*/
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallOutgoingInit,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallOutgoingProgress,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallIncomingReceived,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallOutgoingRinging,1,2000));
	linphone_core_accept_call(laure->lc,linphone_core_get_current_call(laure->lc));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallConnected,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallStreamsRunning,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallConnected,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallStreamsRunning,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallConnected,1,2000));

	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallEnd,1,2000));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
	linphone_core_manager_destroy(laure);
	ms_list_free(lcs);
}

static void unattended_call_transfer_with_error(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCall* pauline_called_by_marie;

	MSList* lcs=ms_list_append(NULL,marie->lc);
	lcs=ms_list_append(lcs,pauline->lc);

	CU_ASSERT_TRUE(call(marie,pauline));
	pauline_called_by_marie=linphone_core_get_current_call(marie->lc);

	reset_counters(&marie->stat);
	reset_counters(&pauline->stat);

	linphone_core_transfer_call(marie->lc,pauline_called_by_marie,"unknown_user");
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallRefered,1,2000));
	
	/*Pauline starts the transfer*/
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallOutgoingInit,1,2000));
	/* and immediately get an error*/
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallError,1,2000));
	
	/*the error must be reported back to marie*/
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneTransferCallError,1,2000));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
	ms_list_free(lcs);
}


static void call_transfer_existing_call_outgoing_call(void) {
	LinphoneCoreManager* marie = linphone_core_manager_new( "marie_rc");
	LinphoneCoreManager* pauline = linphone_core_manager_new( "pauline_rc");
	LinphoneCoreManager* laure = linphone_core_manager_new( "laure_rc");

	LinphoneCall* marie_call_pauline;
	LinphoneCall* pauline_called_by_marie;
	LinphoneCall* marie_call_laure;
	LinphoneCall* laure_called_by_marie;
	LinphoneCall* lcall;

	MSList* lcs=ms_list_append(NULL,marie->lc);
	const MSList* calls;
	lcs=ms_list_append(lcs,pauline->lc);
	lcs=ms_list_append(lcs,laure->lc);

	/*marie call pauline*/
	CU_ASSERT_TRUE(call(marie,pauline));
	marie_call_pauline=linphone_core_get_current_call(marie->lc);
	pauline_called_by_marie=linphone_core_get_current_call(pauline->lc);
	/*marie pause pauline*/
	CU_ASSERT_TRUE(pause_call_1(marie,marie_call_pauline,pauline,pauline_called_by_marie));

	/*marie call laure*/
	CU_ASSERT_TRUE(call(marie,laure));
	marie_call_laure=linphone_core_get_current_call(marie->lc);
	laure_called_by_marie=linphone_core_get_current_call(laure->lc);
	/*marie pause pauline*/
	CU_ASSERT_TRUE(pause_call_1(marie,marie_call_laure,laure,laure_called_by_marie));

	reset_counters(&marie->stat);
	reset_counters(&pauline->stat);
	reset_counters(&laure->stat);


	linphone_core_transfer_call_to_another(marie->lc,marie_call_pauline,marie_call_laure);
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallRefered,1,2000));

	/*pauline pausing marie*/
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallPausing,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallPaused,1,2000));
	/*pauline calling laure*/
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallOutgoingProgress,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneTransferCallOutgoingInit,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallIncomingReceived,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallOutgoingRinging,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneTransferCallOutgoingProgress,1,2000));

	/*laure accept call*/
	for(calls=linphone_core_get_calls(laure->lc);calls!=NULL;calls=calls->next) {
		lcall = (LinphoneCall*)calls->data;
		if (linphone_call_get_state(lcall) == LinphoneCallIncomingReceived) {
			CU_ASSERT_EQUAL(linphone_call_get_replaced_call(lcall),laure_called_by_marie);
			linphone_core_accept_call(laure->lc,lcall);
			break;
		}
	}
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallConnected,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallStreamsRunning,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallConnected,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallStreamsRunning,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneTransferCallConnected,1,2000));

	/*terminate marie to pauline/laure call*/
	CU_ASSERT_TRUE(wait_for_list(lcs,&pauline->stat.number_of_LinphoneCallEnd,1,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&marie->stat.number_of_LinphoneCallEnd,2,2000));
	CU_ASSERT_TRUE(wait_for_list(lcs,&laure->stat.number_of_LinphoneCallEnd,1,2000));

	linphone_core_manager_destroy(marie);
	linphone_core_manager_destroy(pauline);
	linphone_core_manager_destroy(laure);
	ms_list_free(lcs);
}

#ifdef VIDEO_ENABLED
#endif

test_t call_tests[] = {
	{ "Early declined call", early_declined_call },
	{ "Call declined", call_declined },
	{ "Cancelled call", cancelled_call },
	{ "Early cancelled call", early_cancelled_call},
	{ "Call with DNS timeout", call_with_dns_time_out },
	{ "Cancelled ringing call", cancelled_ringing_call },
	{ "Simple call", simple_call },
	{ "Simple call compatibility mode", simple_call_compatibility_mode },
	{ "Early-media call", early_media_call },
	{ "Call terminated by caller", call_terminated_by_caller },
	{ "Call without SDP", call_with_no_sdp},
	{ "Call paused resumed", call_paused_resumed },
	{ "Call paused resumed from callee", call_paused_resumed_from_callee },
	{ "SRTP call", srtp_call },
	{ "SRTP call with declined srtp", call_with_declined_srtp },
#ifdef VIDEO_ENABLED
	{ "Simple video call",video_call},
	{ "SRTP ice video call", srtp_video_ice_call },
	{ "Call with video added", call_with_video_added },
	{ "Call with video declined",call_with_declined_video},
#else
	{ "SRTP ice call", srtp_ice_call },
#endif
	{ "Call with privacy", call_with_privacy },
	{ "Simple conference", simple_conference },
	{ "Simple call transfer", simple_call_transfer },
	{ "Unattended call transfer", unattended_call_transfer },
	{ "Unattended call transfer with error", unattended_call_transfer_with_error },
	{ "Call transfer existing call outgoing call", call_transfer_existing_call_outgoing_call },
	{ "Call with ICE", call_with_ice },
	{ "Call with custom headers",call_with_custom_headers}
};

test_suite_t call_test_suite = {
	"Call",
	NULL,
	NULL,
	sizeof(call_tests) / sizeof(call_tests[0]),
	call_tests
};

