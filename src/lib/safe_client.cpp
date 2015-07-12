#include <signal.h>
#include <openssl/x509.h>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <log.h>
#include <safe_client.h>
#include <safe_storage_net.h>

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
	
#define PREPARE_PREQ(T,P) \
	T *P = (T *)(dbegin + sizeof(CSAFE_NET_HEADER)); \
	memset(P, 0, sizeof(P[0]));
	
CSafeClient::CSafeClient ( void )
{
}

CSafeClient::~CSafeClient ( void )
{
}

int32_t CSafeClient::close ( uint32_t flags )
{
	return sendRequest(SAFE_CMD_CLOSE, flags);
}

int32_t CSafeClient::open ( const string &location, uint32_t flags, uint32_t hash_key )
{
	PREPARE_PREQ(CSAFE_NET_REQ_OPEN, req);

	req->hash_key = hash_key;
	return sendRequest(SAFE_CMD_OPEN, flags, req, sizeof(*req), location.c_str(), location.size()+1);
}

int32_t CSafeClient::commit ( void )
{
	return sendRequest(SAFE_CMD_COMMIT);
}

int32_t CSafeClient::getInfo ( CSafeStorageInfo &info )
{
	int32_t res = sendRequest(SAFE_CMD_GET_INFO);

	CSAFE_NET_RESP_GET_INFO *resp = (CSAFE_NET_RESP_GET_INFO *)respdata;
	if (res == E_CSTORAGE_OK) {
		info = resp->info;
	}
	return res;
}

int32_t CSafeClient::rollback ( void )
{
	return sendRequest(SAFE_CMD_ROLLBACK);
}

int32_t CSafeClient::verify ( uint32_t flags )
{
	return sendRequest(SAFE_CMD_VERIFY, flags);
}


int32_t CSafeClient::write ( tserial_t serial, const void *data, uint32_t dlen, uint32_t flags )
{
	PREPARE_PREQ(CSAFE_NET_REQ_WRITE, req);
	
	req->serial = serial;
	return sendRequest(SAFE_CMD_WRITE, flags, req, sizeof(*req), data, dlen);
}

int32_t CSafeClient::read ( tserial_t &serial, void *data, uint32_t dlen, uint32_t flags )
{
	PREPARE_PREQ(CSAFE_NET_REQ_READ, req);

	req->serial = serial;
	int32_t res = sendRequest(SAFE_CMD_READ, flags, req, sizeof(*req), NULL, 0, sizeof(CSAFE_NET_RESP_READ), data, dlen);
	CSAFE_NET_RESP_READ *resp = (CSAFE_NET_RESP_READ *)respdata;
	if (res >= 0) {
		serial = resp->serial;
	}
	return res;
}

int32_t CSafeClient::readLog ( tseq_t seq, void *data, uint32_t dlen, uint32_t flags )
{
	PREPARE_PREQ(CSAFE_NET_REQ_READ_LOG, req);

	req->seq = seq;
	return sendRequest(SAFE_CMD_READ_LOG, flags, req, sizeof(*req), NULL, 0, 0, data, dlen);
}


int32_t CSafeClient::applyLog ( const void *data, uint32_t dlen, uint32_t flags )
{
	return sendRequest(SAFE_CMD_APPLY_LOG, flags, NULL, 0, data, dlen);
}

int32_t CSafeClient::readLogReg ( tseq_t seq, tserial_t &serial, uint8_t &type, uint32_t flags )
{
	PREPARE_PREQ(CSAFE_NET_REQ_READ_LOG_REG, req);
	req->seq = seq;
	int32_t res = sendRequest(SAFE_CMD_READ_LOG_REG, flags, req, sizeof(*req));
	CSAFE_NET_RESP_READ_LOG_REG *resp = (CSAFE_NET_RESP_READ_LOG_REG *)respdata;
	if (res == E_CSTORAGE_OK) {
		serial = resp->serial;
		type = resp->type;		
	}
	return res;
}

int32_t CSafeClient::getParam ( const string &name )
{
	int32_t res = sendRequest(SAFE_CMD_GET_PARAM, 0, NULL, 0, name.c_str(), name.size()+1);
	CSAFE_NET_RESP_GET_PARAM *resp = (CSAFE_NET_RESP_GET_PARAM *)respdata;
	if (res == E_CSTORAGE_OK) {
		res = resp->value;
	}
	return res;
}

int32_t CSafeClient::setParam ( const string &name, int32_t value )
{
	PREPARE_PREQ(CSAFE_NET_REQ_SET_PARAM, req);
	req->value = value;
	return sendRequest(SAFE_CMD_SET_PARAM, 0, req, sizeof(*req), name.c_str(), name.size()+1);
}

int32_t CSafeClient::sendRequest ( uint32_t cmd, uint32_t flags, const void *reqReq, uint32_t reqRegSize, const void *reqVdata, 
								   uint32_t reqVdataLen, uint32_t respRegSize, void *respVdata, uint32_t respVdataSize )
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
