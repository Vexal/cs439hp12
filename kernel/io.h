#ifndef _IO_H_
#define _IO_H_
template<typename T> class OutputStream {
public:
    virtual void put(T v) = 0;
};

template<typename T> class InputStream {
public:
	//InputStream() : counts(0){}//SDebug::printf("asfasdfasdfasdfasdfasdfasdfasdf\n\n\n\n\n\n\n\n\n\n");}
    virtual T get() = 0;
};

#endif
