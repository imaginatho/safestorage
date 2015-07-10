#include <signal.h>
#include <openssl/x509.h>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <log.h>
#include <safe_client.h>
#include <safe_net.h>

#define DECLARE_NET_REQ_RESP(CMD,REQV,RESPV) \
	CSAFE_NET_REQ_##CMD REQV;\
	CSAFE_NET_RESP_##CMD RESPV;\
	memset(&REQV, 0, sizeof(REQV));\
	memset(&RESPV, 0, sizeof(RESPV));\
	REQV.cmd = SAFE_CMD_##CMD;

#define	SEND_AND_RET_NET_REQ_RESP(REQV, RESPV) \
	sendRequest(&REQV, sizeof(REQV), NULL, 0, &RESPV, sizeof(RESPV)); \
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, RESPV.result);\
	return (RESPV.result);
	
CSafeClient::CSafeClient ( void )
{
}

CSafeClient::~CSafeClient ( void )
{
}

int32_t CSafeClient::close ( uint32_t flags )
{
	DECLARE_NET_REQ_RESP(CLOSE, req, resp)
	C_LOG_TRACE("%s(%08X)", __FUNCTION__, flags);

	req.flags = flags;
	
	SEND_AND_RET_NET_REQ_RESP(req, resp)
}

int32_t CSafeClient::open ( const string &location, uint32_t flags, uint32_t hash_key )
{
	DECLARE_NET_REQ_RESP(OPEN, req, resp)
	C_LOG_TRACE("%s(%s, %08X)", __FUNCTION__, location.c_str(), flags);

	req.flags = flags;
	req.hash_key = hash_key;
	req.location_len = location.size()+1;
	
	sendRequest(&req, sizeof(req), location.c_str(), req.location_len, &resp, sizeof(resp));
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, resp.result);
	return resp.result;
}

int32_t CSafeClient::commit ( void )
{
	DECLARE_NET_REQ_RESP(COMMIT, req, resp)
	C_LOG_TRACE("%s()", __FUNCTION__);

	SEND_AND_RET_NET_REQ_RESP(req, resp)
}

int32_t CSafeClient::getInfo ( CSafeStorageInfo &info )
{
	DECLARE_NET_REQ_RESP(GET_INFO, req, resp)
	C_LOG_TRACE("%s()", __FUNCTION__);

	sendRequest(&req, sizeof(req), NULL, 0, &resp, sizeof(resp));
	if (resp.result == 0) {
		info = resp.info;
	}
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, resp.result);
	return resp.result;
}

int32_t CSafeClient::rollback ( void )
{
	DECLARE_NET_REQ_RESP(ROLLBACK, req, resp)
	C_LOG_TRACE("%s()", __FUNCTION__);

	sendRequest(&req, sizeof(req), NULL, 0, &resp, sizeof(resp));
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, resp.result);
	return resp.result;
}

int32_t CSafeClient::verify ( uint32_t flags )
{
	DECLARE_NET_REQ_RESP(VERIFY, req, resp)
	C_LOG_TRACE("%s(%08X)", __FUNCTION__, flags);

	req.flags = flags;
	
	SEND_AND_RET_NET_REQ_RESP(req, resp)
}


int32_t CSafeClient::write ( tserial_t serial, const void *data, uint32_t dlen, uint32_t flags )
{
	DECLARE_NET_REQ_RESP(WRITE, req, resp)
	C_LOG_TRACE("%s(%u,..,%u,%08X)", __FUNCTION__, serial, dlen, flags);

	req.serial = serial;
	req.dlen = dlen;
	req.flags = flags;
	
	sendRequest(&req, sizeof(req), data, req.dlen, &resp, sizeof(resp));
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, resp.result);
	return resp.result;
}

int32_t CSafeClient::read ( tserial_t &serial, void *data, uint32_t dlen, uint32_t flags )
{
	DECLARE_NET_REQ_RESP(READ, req, resp)
	C_LOG_TRACE("%s(%u,..,%u,%08X)", __FUNCTION__, serial, dlen, flags);

	req.serial = serial;
	req.dlen = dlen;
	req.flags = flags;
	
	sendRequest(&req, sizeof(req), NULL, 0, &resp, sizeof(resp), data, &resp.result);
	if (resp.result == 0) {
		serial = resp.serial;
	}
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, resp.result);
	return resp.result;
}

int32_t CSafeClient::readLog ( tseq_t seq, void *data, uint32_t dlen, uint32_t flags )
{
	DECLARE_NET_REQ_RESP(READ_LOG, req, resp)
	C_LOG_TRACE("%s(%u,..,%u,%08X)", __FUNCTION__, seq, dlen, flags);

	req.seq = seq;
	req.dlen = dlen;
	req.flags = flags;

	sendRequest(&req, sizeof(req), NULL, 0, &resp, sizeof(resp), data, &resp.result);
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, resp.result);
	return resp.result;
}


int32_t CSafeClient::applyLog ( const void *data, uint32_t dlen, uint32_t flags )
{
	DECLARE_NET_REQ_RESP(APPLY_LOG, req, resp)
	C_LOG_TRACE("%s(..,%u,%08X)", __FUNCTION__,  dlen, flags);

	req.dlen = dlen;
	req.flags = flags;

	sendRequest(&req, sizeof(req), data, dlen, &resp, sizeof(resp));
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, resp.result);
	return resp.result;
}

int32_t CSafeClient::readLogReg ( tseq_t seq, tserial_t &serial, uint8_t &type, uint32_t flags )
{
	DECLARE_NET_REQ_RESP(READ_LOG_REG, req, resp)
	C_LOG_TRACE("%s(%d,..,%08X)", __FUNCTION__, serial, flags);

	req.seq = seq;
	req.flags = flags;

	sendRequest(&req, sizeof(req), NULL, 0, &resp, sizeof(resp));
	if (resp.result == 0) {
		serial = resp.serial;
		type = resp.type;
	}
	
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, resp.result);
	return resp.result;
}


int32_t CSafeClient::goTop ( uint32_t flags ) 
{
	DECLARE_NET_REQ_RESP(GO_TOP, req, resp)
	C_LOG_TRACE("%s(%08X)", __FUNCTION__, flags);

	req.flags = flags;
	SEND_AND_RET_NET_REQ_RESP(req, resp)
}


int32_t CSafeClient::goPos ( tserial_t serial, uint32_t flags ) 
{
	DECLARE_NET_REQ_RESP(GO_POS, req, resp)
	C_LOG_TRACE("%s(%d,%08X)", __FUNCTION__, serial, flags);

	req.flags = flags;
	req.serial = serial;
	SEND_AND_RET_NET_REQ_RESP(req, resp)
}


int32_t CSafeClient::getParam ( const string &name )
{
	DECLARE_NET_REQ_RESP(GET_PARAM, req, resp)
	C_LOG_TRACE("%s(%s)", __FUNCTION__, name.c_str());

	req.name_len = name.size()+1;
	sendRequest(&req, sizeof(req), name.c_str(), req.name_len, &resp, sizeof(resp));
	
	if (resp.result == 0) {
		return resp.value;
	}
	
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, resp.result);
	return resp.result;
}

int32_t CSafeClient::setParam ( const string &name, int32_t value )
{
	DECLARE_NET_REQ_RESP(SET_PARAM, req, resp)
	C_LOG_TRACE("%s(%s,%d)", __FUNCTION__, name.c_str(), value);

	req.name_len = name.size()+1;
	req.value = value;
	sendRequest(&req, sizeof(req), name.c_str(), req.name_len, &resp, sizeof(resp));
	C_LOG_DEBUG("%s.result:%d", __FUNCTION__, resp.result);
	return resp.result;
}

int32_t CSafeClient::sendRequest ( const void *req, uint32_t reqlen, const void *req_vdata, uint32_t req_vdatalen, void *resp, uint32_t resplen, 
								   const void *resp_vdata, int32_t *resp_vdatalen )
{
/*
	C_LOG_DEBUG("sendRequest msg_in.size:%d", msg_in.GetSize());
	
	TMem in;
	in.write(msg_in.GetData(), msg_in.GetSize());
	msg_in.GoTop();
	
	waiting = true;
	int32_t result = TPacketClient::sendRequest(in, out, TRelTime(1));
	//int32_t result = TPacketClient::sendMessage(in);
	// while (waiting);
	msg_out.Set(out.getData(), out.getSize());
	msg_out.GoTop();
		
	C_LOG_DEBUG("sendRequest result:%d msg_out.size:%d", result, out.getSize());
*/	
	return 0;
}
