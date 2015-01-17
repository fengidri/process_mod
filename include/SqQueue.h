/********************************************************
  @   Author: 丁雪峰
  @     File: SqQueue.h
  @ LastTime: 2013-08-07 18:41:07
  @  Version: 1.0.3
  Description:
      循环数据队列,在队列满时,插入数据时,队首出队,第二个元素
      成为队首,队尾指向原队首.不断加入新的数据,不会溢出,只保留
      QUEUE_MAX_LEN个数的元素.

      使用泛型进行设计.

      是CProcess的基本组成,也是整个项目中的数据传递单元.
********************************************************/
#ifndef _SQ_QUEUE
#define _SQ_QUEUE

#include "stdmacro.h"
#include "string.h"
#include <list>
using namespace std;

//^^^^^^^^^^^^^^^^^^[START:类定义]^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
template <class T>                                              //
class CSqQueue                                                  //
{                                                               //
    private:                                                    //
        T *m_base;//数据池                                      //
        unsigned int m_uRear;//队尾                             //
        unsigned int m_uFront;//队首                            //
        unsigned int m_uSize;//队列元素的个数                   //
        unsigned int m_uIterator;                               //
        bool m_bIteratorBool;                                   //
                                                                //
    public:                                                     //
        CSqQueue();                                             //
        ~CSqQueue();//初始化                                    //
        bool Push(T );//入队                                    //
        bool PushPreMinus(T element );                          //
        bool IsEmpty();//判断队列是否为空                       //
        void Clear();                                           //
        T Tail() const;                                         //
        T LastButOne() const;                                   //
                                                                //
        //iterator                                              //
        T* Begin();                                            //
        T* End();                                              //
        T *Iterator;                                          //

        int Size() const;
        //返回相对于队首偏移了offset的的值
        T Offset(int offset) const;
                                                                //
        list<T> List () const;                                  //
};                                                              //
                                                                //
//___________________[END]______________________________________//

/****************************************************************
 * Function:CSqQueue
 * Description:
 *    新建一个空的队列,并设置初始值是 0
 *    并初始化队首队尾
 * Input:无
 * Output:
 * Return:
 * Others:
****************************************************************/
template<class T>
CSqQueue<T>::CSqQueue()
{ 
    // 构造一个空队列
    m_uSize = QUEUE_MAX_LEN;
    m_base = new T[m_uSize];
    memset(m_base, 0, sizeof(T)*m_uSize);
    m_uFront = m_uRear = 0;
    
}
template<class T>
CSqQueue<T>::~CSqQueue()
{ 
    delete [] m_base;
}


template<class T>
void CSqQueue<T>::Clear()
{ 
    /* 将Q清为空队列 */
    m_uFront = m_uRear = 0;
}

template<class T>
bool CSqQueue<T>::IsEmpty()
{ 
    if(m_uFront == m_uRear) /* 队列空的标志 */
    { 
        return TRUE;
    }
    else
    { 
        return FALSE;
    }
}

/****************************************************************
 * Function:push
 * Description:
 *     插入新元素,在队列不满的时候,直接插入,在队列满时,自动在队首的
 *     位置插入新元素,并向后移动队首
 * Input:新元素,其类型与队列在初始化时相同
 * Output:在满地情况下返回0,
 *     在不满地情况下返回1
 * Return:
 * Others:
****************************************************************/
template<class T>
bool CSqQueue<T>::Push(T element)
{ 
    /* 插入元素e为的新的队尾元素 */
    if((m_uRear+1)%m_uSize == m_uFront) /* 队列满 */
    { 
        m_base[m_uRear] = element;
        m_uFront = (m_uFront + 1)%m_uSize;
        m_uRear = (m_uRear + 1)%m_uSize;
        return 0;
    }
    else
    { 
        m_base[m_uRear] = element;
        m_uRear = (m_uRear+1)%m_uSize;
        return 1;
    }
}

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^[START:迭代器]^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^//
// Function:iterator_init  iterator_bool  r_next                             //
// Description:                                                              //
//    三个函数组合使用可以从最后一个元素开始一个一个地返回队列里的元素       //
//                                                                           //
//    首先使用iterator_init,使用迭代从队尾开始.                              //
//    iterator_bool可以判断有没有迭代完成                                    //
//    r_next可以依次返回队列里的元素                                         //
// Input:                                                                    //
//    三个函数组合使用:                                                      //
//    r_next返回元素                                                         //
//    iterator_bool返回布尔值                                                //
//    iterator_init返回0                                                     //
// Output:                                                                   //
// Return:                                                                   //
// Others:                                                                   //
template<class T>                                                            //
T* CSqQueue<T>::Begin()                                                      //
{                                                                            //
    return m_base;                                                           //
}                                                                            //
                                                                             //
template<class T>                                                            //
T* CSqQueue<T>::End()                                                        //
{                                                                            //
    return m_base + m_uRear;                                                 //
}                                                                            //
                                                                             //
//___________________________[END]___________________________________________//

/****************************************************************
 * Function:tail
 * Description:
 *    返回新插入的元素,也就是队尾指向的前一个元素
 * Input:无
 * Output:
 *    返回新插入的元素
 * Return:
 * Others:
****************************************************************/
template<class T>
T CSqQueue<T>::Tail () const
{ 
    return m_base[(m_uRear - 1 + m_uSize) % m_uSize];
}
/****************************************************************
 * Function:LastButOne
 * Description:
 *    返回新插入的元素前面的一个元素,主要在计算cpu时会用到
 * Input:无
****************************************************************/
template<class T>
T CSqQueue<T>::LastButOne() const
{ 
    return m_base[(m_uRear - 2 + m_uSize) % m_uSize];
}

/****************************************************************
 * Description:
 *     返回返回一个队列里的所有元素的list对象
****************************************************************/
template<class T>
list<T> CSqQueue<T>::List () const
{ 
    list<T> TmpList;
    unsigned int i;
    for (i = m_uFront; i != m_uRear; i = (i + 1)%m_uSize)
    { 

        TmpList.push_back(m_base[i]);
    }

    return TmpList;
}

template<class T>
T CSqQueue<T>::Offset(int offset) const
{ 
    return m_base[(m_uFront  + offset) % m_uSize];
}

template<class T>
int CSqQueue<T>::Size() const
{ 
    if (m_uRear > m_uFront)
    { 
        return m_uRear  - m_uFront;
    }
    else
    { 
        return m_uSize - m_uFront + m_uRear;

    }
}
#endif
