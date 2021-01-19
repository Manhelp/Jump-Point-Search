template < typename T>
class Singleton
{
private:
	Singleton(const Singleton&);
	const Singleton & operator= (const Singleton &);
protected:
	static T* ms_Singleton;
	Singleton(void)
	{
	}

	~Singleton(void)
	{
	}
public:

	static __inline T* instance(void)
	{
        if(!ms_Singleton)
        {
            ms_Singleton = new T();
        }

		return ms_Singleton;
	}
};


template <typename T>
T* Singleton<T>::ms_Singleton = 0;