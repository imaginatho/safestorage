#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <stdint.h>
#include <stdexcept>
#include <stdarg.h>

using namespace std;

class CException: public exception
{
    protected:
        string message;
        string file;
        uint32_t line;
        int32_t result;
		int32_t message_pos;
		void init ( const char *ref_format, const char *file, uint32_t line, int32_t *result, const char *format, va_list va_args ) throw();
        void init ( int32_t *result, const char *format, ... ) throw();
		
    public:
        CException ( const char *file, uint32_t line, const char *format, ... ) throw();
        CException ( const char *file, uint32_t line, int32_t result, const char *format, ... ) throw();
        CException ( const char *file, uint32_t line, int32_t result ) throw();
		CException ( const char *file, uint32_t line ) throw();
        virtual ~CException ( void ) throw();
        const char *getMessage ( void ) const;
        const char *getFile ( void ) const;
        int32_t getLine ( void ) const;
        int32_t getResult ( void ) const;
        virtual const char *what ( void ) const throw();
};


#ifndef CEXP_MACRO_CONCAT_AUX 
#define CEXP_MACRO_CONCAT_AUX(X,Y) X ## Y
#endif

#ifndef CEXP_MACRO_CONCAT 
#define CEXP_MACRO_CONCAT(X,Y) CEXP_MACRO_CONCAT_AUX(X,Y)
#endif

#define CEXP_ASSERT(X,...) if(!(X)) throw CException(__FILE__,__LINE__,__VA_ARGS__)
#define CEXP_N_ASSERT(X,N,...) if(!(X)) throw CException(__FILE__,__LINE__,N,__VA_ARGS__)

#define CEXP(...) throw CException(__FILE__,__LINE__,__VA_ARGS__)
#define CEXP_ASSERT_RESULT(X,...) { int result; if((result = X)) throw CException(__FILE__,__LINE__,__VA_ARGS__); }
#define CEXP_ASSERT_RESULT_VALUE(X,RS,...) { int result; if((result = X) != RS) throw CException(__FILE__,__LINE__,__VA_ARGS__); }

#define CEXP_CODE(X) 			{ throw CException(__FILE__,__LINE__, X); }
#define CEXP_CODE_T(X,...) 		{ throw CException(__FILE__,__LINE__, X, __VA_ARGS__); }
#define CEXP_CODE_SYS(X,...) 	{ throw CException(__FILE__,__LINE__, X, "SYS(%d) %s", errno, strerror(errno)); }
#define CEXP_SYS(X,...) 		{ throw CException(__FILE__,__LINE__, "SYS(%d) %s", errno, strerror(errno)); }

#endif

