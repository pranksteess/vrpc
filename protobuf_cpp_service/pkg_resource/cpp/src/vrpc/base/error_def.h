/*
 * error_def.h
 *
 *  Created on: 2013-5-17
 *      Author: ylc8131
 */

#ifndef ERROR_DEF_H_
#define ERROR_DEF_H_

//L5 ok
#define L5_OK 0

//L5 error
#define L5_ERROR (-1)

namespace vrpc
{
namespace base
{
    enum ErrorCode
    {
        API_SOCKINIT_ERR = -101,
        API_SOCKSEND_ERR = -102,
        API_SOCKRECVLEN_ERR = -103,
        API_SOCKRECVBODY_ERR = -104,
        API_PARSERESP_ERR = -105,
        API_RESPHEAD_ERR = -106,
        API_BUFOVERFLOW_ERR = -107
    };
}
}


#endif /* ERROR_DEF_H_ */
