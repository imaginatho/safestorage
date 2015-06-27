#ifndef __SAFE_CMDS_H__
#define __SAFE_CMDS_H__

#define SAFE_CMD_OPEN			1
#define SAFE_CMD_CLOSE			2
#define SAFE_CMD_READ			3
#define SAFE_CMD_WRITE			4
#define SAFE_CMD_VERIFY			5
#define SAFE_CMD_READ_LOG		6
#define SAFE_CMD_APPLY_LOG		7
#define SAFE_CMD_READ_LOG_REG   8
#define SAFE_CMD_COMMIT			9
#define SAFE_CMD_ROLLBACK		10
#define SAFE_CMD_GO_TOP			11
#define SAFE_CMD_GO_POS			12
#define SAFE_CMD_GET_PARAM		13
#define SAFE_CMD_SET_PARAM		14
#define SAFE_CMD_GET_STATE		15

inline const char *safeCmd2String ( uint32_t cmd );

// OPEN

typedef struct
{
	uint8_t cmd;
	uint32_t flags;
	uint32_t hash_key;
	uint16_t location_len;
} __attribute__((packed)) CSAFE_NET_REQ_OPEN;

typedef struct
{
	int32_t result;
} __attribute__((packed)) CSAFE_NET_RESP_OPEN;

// CLOSE

typedef struct
{
	uint8_t cmd;
	uint32_t flags;
} __attribute__((packed)) CSAFE_NET_REQ_CLOSE;

typedef struct
{
	int32_t result;
} __attribute__((packed)) CSAFE_NET_RESP_CLOSE;


// COMMIT

typedef struct
{
	uint8_t cmd;
} __attribute__((packed)) CSAFE_NET_REQ_COMMIT;

typedef struct
{
	int32_t result;
} __attribute__((packed)) CSAFE_NET_RESP_COMMIT;

// GET_STATE

typedef struct
{
	uint8_t cmd;
} __attribute__((packed)) CSAFE_NET_REQ_GET_STATE;

typedef struct
{
	int32_t result;
	CSafeStorageState state;
} __attribute__((packed)) CSAFE_NET_RESP_GET_STATE;

// ROLLBACK

typedef struct
{
	uint8_t cmd;
} __attribute__((packed)) CSAFE_NET_REQ_ROLLBACK;

typedef struct
{
	int32_t result;
} __attribute__((packed)) CSAFE_NET_RESP_ROLLBACK;

// VERIFY

typedef struct
{
	uint8_t cmd;
	uint32_t flags;
} __attribute__((packed)) CSAFE_NET_REQ_VERIFY;

typedef struct
{
	int32_t result;
} __attribute__((packed)) CSAFE_NET_RESP_VERIFY;

// WRITE

typedef struct
{
	uint8_t cmd;
	tserial_t serial;
	int32_t dlen;
	uint32_t flags;
} __attribute__((packed)) CSAFE_NET_REQ_WRITE;

typedef struct
{
	int32_t result;
} __attribute__((packed)) CSAFE_NET_RESP_WRITE;

// READ

typedef struct
{
	uint8_t cmd;
	tserial_t serial;
	int32_t dlen;
	uint32_t flags;
} __attribute__((packed)) CSAFE_NET_REQ_READ;

typedef struct
{
	int32_t result;
	tserial_t serial;
} __attribute__((packed)) CSAFE_NET_RESP_READ;

// READ_LOG 

typedef struct
{
	uint8_t cmd;
	tseq_t seq;
	int32_t dlen;
	uint32_t flags;
} __attribute__((packed)) CSAFE_NET_REQ_READ_LOG;

typedef struct
{
	int32_t result;
	CSafeStorageDataReg reg;
} __attribute__((packed)) CSAFE_NET_RESP_READ_LOG;

// APPLY_LOG

typedef struct
{
	uint8_t cmd;
	CSafeStorageDataReg reg;
	uint32_t flags;
	int32_t dlen;
} __attribute__((packed)) CSAFE_NET_REQ_APPLY_LOG;

typedef struct
{
	int32_t result;
} __attribute__((packed)) CSAFE_NET_RESP_APPLY_LOG;

// READ_LOG_REG

typedef struct
{
	uint8_t cmd;
	tseq_t seq;
	uint32_t flags;
} __attribute__((packed)) CSAFE_NET_REQ_READ_LOG_REG;

typedef struct
{
	int32_t result;
	tserial_t serial;
	uint8_t type;
} __attribute__((packed)) CSAFE_NET_RESP_READ_LOG_REG;

// GO_TOP

typedef struct
{
	uint8_t cmd;
	uint32_t flags;
} __attribute__((packed)) CSAFE_NET_REQ_GO_TOP;

typedef struct
{
	int32_t result;
} __attribute__((packed)) CSAFE_NET_RESP_GO_TOP;

// GO_POS

typedef struct
{
	uint8_t cmd;
	uint32_t flags;
	tserial_t serial;
} __attribute__((packed)) CSAFE_NET_REQ_GO_POS;

typedef struct
{
	int32_t result;
} __attribute__((packed)) CSAFE_NET_RESP_GO_POS;

// GET_PARAM

typedef struct
{
	uint8_t cmd;
	uint32_t name_len;
} __attribute__((packed)) CSAFE_NET_REQ_GET_PARAM;

typedef struct
{
	int32_t result;
	int32_t value;
} __attribute__((packed)) CSAFE_NET_RESP_GET_PARAM;

// SET_PARAM

typedef struct
{
	uint8_t cmd;
	uint32_t name_len;
	int32_t value;
} __attribute__((packed)) CSAFE_NET_REQ_SET_PARAM;

typedef struct
{
	int32_t result;
} __attribute__((packed)) CSAFE_NET_RESP_SET_PARAM;

inline const char *safeCmd2String ( uint32_t cmd )
{
	switch (cmd) {
		case SAFE_CMD_OPEN:	        return "SAFE_CMD_OPEN";	
		case SAFE_CMD_CLOSE:		return "SAFE_CMD_CLOSE";	
		case SAFE_CMD_READ:	        return "SAFE_CMD_READ";	
		case SAFE_CMD_WRITE:	    return "SAFE_CMD_WRITE";	
		case SAFE_CMD_VERIFY:       return "SAFE_CMD_VERIFY";
		case SAFE_CMD_READ_LOG:     return "SAFE_CMD_READ_LOG";
		case SAFE_CMD_READ_LOG_REG: return "SAFE_CMD_READ_LOG_REG";
		case SAFE_CMD_COMMIT:       return "SAFE_CMD_COMMIT";
		case SAFE_CMD_ROLLBACK:     return "SAFE_CMD_ROLLBACK";
		case SAFE_CMD_GO_TOP:	    return "SAFE_CMD_GO_TOP";	
		case SAFE_CMD_GO_POS:       return "SAFE_CMD_GO_POS";
		case SAFE_CMD_GET_PARAM:    return "SAFE_CMD_GET_PARAM";
		case SAFE_CMD_SET_PARAM:    return "SAFE_CMD_SET_PARAM";
		case SAFE_CMD_APPLY_LOG:    return "SAFE_CMD_APPLY_LOG";
		case SAFE_CMD_GET_STATE:    return "SAFE_CMD_GET_STATE";
	}
	return "SAFE_CMD_XXX";
}

#endif


