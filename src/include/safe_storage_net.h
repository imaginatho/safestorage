#ifndef __SAFE_STORAGE_NET_H__
#define __SAFE_STORAGE_NET_H__

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
#define SAFE_CMD_GET_PARAM		13
#define SAFE_CMD_SET_PARAM		14
#define SAFE_CMD_GET_INFO		15

inline const char *safeCmd2String ( uint32_t cmd );

#define SAFE_NET_HEADER_SIGNATURE		0x23C4B955
typedef struct
{
    uint32_t signature;
    uint32_t len;
	uint32_t flags;
	int32_t  cmd_res;
} __attribute__ ((packed)) CSAFE_NET_HEADER;

// OPEN

typedef struct
{
	uint32_t hash_key;
} __attribute__((packed)) CSAFE_NET_REQ_OPEN;

// GET_INFO

typedef struct
{
	CSafeStorageInfo info;
} __attribute__((packed)) CSAFE_NET_RESP_GET_INFO;

// WRITE

typedef struct
{
	tserial_t serial;
} __attribute__((packed)) CSAFE_NET_REQ_WRITE;

// READ

typedef struct
{
	tserial_t serial;
} __attribute__((packed)) CSAFE_NET_REQ_READ;

typedef struct
{
	tserial_t serial;
} __attribute__((packed)) CSAFE_NET_RESP_READ;

// READ_LOG 

typedef struct
{
	tseq_t seq;
} __attribute__((packed)) CSAFE_NET_REQ_READ_LOG;

// READ_LOG_REG

typedef struct
{
	tseq_t seq;
} __attribute__((packed)) CSAFE_NET_REQ_READ_LOG_REG;

typedef struct
{
	tserial_t serial;
	uint8_t type;
} __attribute__((packed)) CSAFE_NET_RESP_READ_LOG_REG;

// GET_PARAM

typedef struct
{
	int32_t value;
} __attribute__((packed)) CSAFE_NET_RESP_GET_PARAM;

// SET_PARAM

typedef struct
{
	int32_t value;
} __attribute__((packed)) CSAFE_NET_REQ_SET_PARAM;

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
		case SAFE_CMD_GET_PARAM:    return "SAFE_CMD_GET_PARAM";
		case SAFE_CMD_SET_PARAM:    return "SAFE_CMD_SET_PARAM";
		case SAFE_CMD_APPLY_LOG:    return "SAFE_CMD_APPLY_LOG";
		case SAFE_CMD_GET_INFO:     return "SAFE_CMD_GET_INFO";
	}
	return "SAFE_CMD_XXX";
}

#endif


