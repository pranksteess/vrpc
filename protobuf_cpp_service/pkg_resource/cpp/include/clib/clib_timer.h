/**
 * @file    clib_timer.h
 * @version 1.0
 * @author  rogeryang
 * @date    2012/12/17
 * @brief   耗时测量辅助类/函数/宏
 */
#ifndef _CLIB_TIMER_H_
#define _CLIB_TIMER_H_
#include <map>
#include <string>
#include <sys/time.h>

/** @brief 测量耗时的工具类
 *         用start()启动一个测量点， end()停止一个测量点，
 *         get_ms/get_us获取start到end间的耗时
 *         用字符串标识每个测量点
 *
 */
class clib_timer
{
public:
    clib_timer();
    ~clib_timer();

    /** @brief 启动一个测量点
     *
     *  @param[in] as_name 测量点的名字
     *  @return 无
     */
    void start( const char* as_name );

    /** @brief 结束一个测量点
     *
     *  @param[in] as_name 测量点的名字
     *  @return 无
     */
    void end( const char* as_name );

    /** @brief 获取一个测量点的耗时(毫秒)
     *
     *  @param[in] as_name 测量点的名字
     *  @return 测量点的耗时, 如果测量点不存在, 返回0
     */
    uint32_t get_ms( const char* as_name );

    /** @brief 获取一个测量点的耗时(微秒)
     *
     *  @param[in] as_name 测量点的名字
     *  @return 测量点的耗时, 如果测量点不存在, 返回0
     */
    uint32_t get_us( const char* as_name );

    /** @brief 结束并获取一个测量点的耗时(毫秒)
     *
     *  @param[in] as_name 测量点的名字
     *  @return 测量点的耗时, 如果测量点不存在, 返回0
     */
    uint32_t end_get_ms( const char* as_name );

    /** @brief 结束并获取一个测量点的耗时(微秒)
     *
     *  @param[in] as_name 测量点的名字
     *  @return 测量点的耗时, 如果测量点不存在, 返回0
     */
    uint32_t end_get_us( const char* as_name );

    /* 测量点信息和map的类型定义 */
    typedef struct _tagTimePointInfo
    {
        struct timeval tvStart;
        struct timeval tvEnd;
        uint32_t uiUseTimeMS;
        uint32_t uiUseTimeUS;
        _tagTimePointInfo():uiUseTimeMS(0), uiUseTimeUS(0)
        {
            memset(&tvStart, 0, sizeof(tvStart) );
            memset(&tvEnd, 0, sizeof(tvEnd) );
        }
    }TimePointInfo;

    typedef std::map<std::string, TimePointInfo> TimerResultMap;
    typedef TimerResultMap::iterator TimerResultMapIter;
    typedef TimerResultMap::const_iterator TimerResultMapConstIter;

    /** @brief 获取所有测量点的数据
     *
     *  @return 所有测量点的map, key为测量点名字, value为测量点信息(TimePointInfo)
     */
    TimerResultMap get_all_timepoint();

private:

    /** @brief 根据测量点信息中的开始结束时间计算耗时，内部使用
     *
     */
    static void cal_result( TimePointInfo& info )
    {
        info.uiUseTimeUS = ( info.tvEnd.tv_sec - info.tvStart.tv_sec)*1000000+info.tvEnd.tv_usec-info.tvStart.tv_usec;
        info.uiUseTimeMS = info.uiUseTimeUS/1000;
    }

    /* 存放测量点信息 */
    TimerResultMap m_mapResult;
};

/** @brief 测量函数/表达式的执行时间
 *         需要传入clib_timer对象和测量点名字
 *
 * @param[in]    func   需要测量执行时间的函数/表达式
 * @param[in]    timer  用来测量时间的clib_timer类对象
 * @param[in]    timer_name 测量点的名字
 * @code
 *      //测量func_test(params)的执行时间
 *      clib_timer timer;
 *      int iRet = 0;
 *      char szParam[1024] = {0};
 *
 *      TIME_PROFILE_CALL( iRet=func_test(szParam), timer, "func_test" );
 *
 *      printf( "func_test ret:%d usetime:%u ms\n", iRet, timer.get_ms("func_test") );
 * @endcode
 */
#define TIME_PROFILE_CALL(func, timer, time_point_name ) \
    do\
    {\
        timer.start(time_point_name);\
        func;\
        timer.end(time_point_name);\
    }while(0)

/** @brief 测量函数/表达式的执行时间(毫秒)
 *         无需传入clib_timer对象和测量点名字
 *
 * @param[in]    func   需要测量执行时间的函数/表达式
 * @param[out]   use_time_ms  用来返回消耗的时间(毫秒)
 * @code
 *      //测量func_test(params)的执行时间
 *      int iRet = 0;
 *      uint32_t uiUseTimeMs = 0;
 *      char szParam[1024] = {0};
 *
 *      TIME_PROFILE_CALL_MS( iRet=func_test(szParam), uiUseTimeMs );
 *
 *      printf( "func_test ret:%d usetime:%u ms\n", iRet, uiUseTimeMs );
 * @endcode
 */
#define TIME_PROFILE_CALL_MS(func, use_time_ms) \
    do\
    {\
        clib_timer timer;\
        static const char* as_tmp_name = "time_profile_call_ms";\
        timer.start(as_tmp_name);\
        func;\
        use_time_ms = timer.end_get_ms(as_tmp_name);\
    }while(0)

/** @brief 测量函数/表达式的执行时间(微秒)
 *         无需传入clib_timer对象和测量点名字
 *
 * @param[in]    func   需要测量执行时间的函数/表达式
 * @param[out]   use_time_us  用来返回消耗的时间(微秒)
 * @code
 *      //测量func_test(params)的执行时间
 *      int iRet = 0;
 *      uint32_t uiUseTimeUs = 0;
 *      char szParam[1024] = {0};
 *
 *      TIME_PROFILE_CALL_US( iRet=func_test(szParam), uiUseTimeUs );
 *
 *      printf( "func_test ret:%d usetime:%u us\n", iRet, uiUseTimeUs );
 * @endcode
 */
#define TIME_PROFILE_CALL_US(func, use_time_us) \
    do\
    {\
        clib_timer timer;\
        static const char* as_tmp_name = "time_profile_call_us";\
        timer.start(as_tmp_name);\
        func;\
        use_time_us = timer.end_get_us(as_tmp_name);\
    }while(0)

inline
clib_timer::clib_timer()
{
}

inline
clib_timer::~clib_timer()
{
}

inline void
clib_timer::start( const char* as_name )
{
    TimePointInfo stTimerInfo;
    gettimeofday( &(stTimerInfo.tvStart), NULL );
    m_mapResult[as_name] = stTimerInfo;
}

inline void
clib_timer::end( const char* as_name )
{
    TimerResultMapIter iter = m_mapResult.find( as_name );
    if( iter != m_mapResult.end() )
    {
        gettimeofday( &(iter->second.tvEnd), NULL );
        cal_result( iter->second );
    }
}

inline uint32_t
clib_timer::get_ms( const char* as_name )
{
    TimerResultMapIter iter = m_mapResult.find( as_name );
    if( iter != m_mapResult.end() )
    {
        return iter->second.uiUseTimeMS;
    }
    else
    {
        return 0;
    }
}

inline uint32_t
clib_timer::get_us( const char* as_name )
{
    TimerResultMapIter iter = m_mapResult.find( as_name );
    if( iter != m_mapResult.end() )
    {
        return iter->second.uiUseTimeUS;
    }
    else
    {
        return 0;
    }
}

inline uint32_t
clib_timer::end_get_ms( const char* as_name )
{
    end( as_name );
    return get_ms( as_name );
}

inline uint32_t
clib_timer::end_get_us( const char* as_name )
{
    end( as_name );
    return get_us( as_name );
}

inline clib_timer::TimerResultMap
clib_timer::get_all_timepoint()
{
    return m_mapResult;
}

#endif

