#ifndef _PST_TEMPL_H_
#define _PST_TEMPL_H_

class PluginBase;
template<typename T>
class MdbEraseAdapter
{
public:
	typedef void (__stdcall PluginBase::*TEraseMethod)(T*);
	MdbEraseAdapter(T* ptr,PluginBase* plg,TEraseMethod method):ptr_(ptr),plugin_(plg),method_(method){}
    void Delete()
	{
		if(plugin_ && method_) (plugin_->*method_)(ptr_);
	}
	T* GetObj()
	{
		return ptr_;
	}
private:
	T* ptr_;
	PluginBase* plugin_;
	TEraseMethod method_;
};


struct MapiEraser
{   
	template<class I>
	static void Delete(I* ptr)
	{
		if(ptr) MAPIFreeBuffer(ptr);
	}
};

struct ComEraser
{   
    template<class I>
	static void Delete(I* ptr)
	{
		if(ptr) ptr->Release();
	}
};

struct RowEraser
{   
	template<class I>
	static void Delete(I* ptr)
	{
		if(ptr) FreeProws(ptr);
	}
};
struct MdbEraser
{
	template<class I>
    static void Delete(I* ptr)
	{
        if(ptr) 
        {
            ptr->Delete();
            delete ptr;
        }

	}
};
namespace COM 
{   
	template<typename I,class Eraser> class AutoPtr
	{
	public:
		explicit AutoPtr( I* Ptr = NULL ) : Ptr_( Ptr ) {}
		I** operator &() //it's using for new allocate 
		{   
			if(Ptr_)Eraser::Delete(Ptr_);
            Ptr_ = 0;
			return &Ptr_; 
		}
		~AutoPtr()
        {
            if(Ptr_)
            {
                Eraser::Delete(Ptr_);
                Ptr_ = 0;
            }
        }
		operator I*() { return Ptr_; }
		I* operator-> () { return Ptr_; }
		I& operator*()
		{   
            if (!Ptr_)
            {
                NL_ERROR("Zero pointer");
                throw std::exception("Zero pointer");
            }
			return *Ptr_;
		}
		I* release()
		{
			I* tmp = Ptr_;
			Ptr_ = 0;
			return tmp;
		}
		I* reset(I* Ptr = 0)
		{
			if(Ptr_) Eraser::Delete(Ptr_);//if(Ptr_!=0)	Ptr_->Release();
			return Ptr_ = Ptr;
		}
        I* get(){ return Ptr_;}
	private:
		I* Ptr_;
    private:
        AutoPtr(const AutoPtr&);
        AutoPtr& operator=(const AutoPtr&);
	}; // class AutoPtr
} // namespace COM

#endif