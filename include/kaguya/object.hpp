// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <string>
#include <cstring>
#include <typeinfo>
#include <algorithm>

#include "kaguya/config.hpp"
#include "kaguya/utility.hpp"
#include "kaguya/traits.hpp"
#include "kaguya/exception.hpp"

namespace kaguya
{
	namespace types
	{
		template<typename T>
		struct typetag {};
	}

	inline void* metatable_name_key() { static int key; return &key; }
	inline void* metatable_type_table_key() { static int key; return &key; }

	template<typename T>
	const std::type_info& metatableType()
	{
		return typeid(typename traits::decay<T>::type);
	}
	template<typename T>
	inline const char* metatableName()
	{
		return metatableType<T>().name();
	}

	struct ObjectWrapperBase
	{
		virtual const void* cget() = 0;
		virtual void* get() = 0;

		virtual const std::type_info& type() = 0;

		virtual const std::type_info& native_type() { return type(); }
		virtual void* native_get() { return get(); }

		ObjectWrapperBase() {}
		virtual ~ObjectWrapperBase() {}
	private:

		//noncopyable
		ObjectWrapperBase(const ObjectWrapperBase&);
		ObjectWrapperBase& operator=(const ObjectWrapperBase&);
	};

	template<class T>
	struct ObjectWrapper : ObjectWrapperBase
	{
#if KAGUYA_USE_CPP11
		template<class... Args>
		ObjectWrapper(Args&&... args) : object_(std::forward<Args>(args)...) {}
#else

		ObjectWrapper() : object_() {}
		template<class Arg1>
		ObjectWrapper(const Arg1& v1) : object_(v1) {}
		template<class Arg1, class Arg2>
		ObjectWrapper(const Arg1& v1, const Arg2& v2) : object_(v1, v2) {}
		template<class Arg1, class Arg2, class Arg3>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3) : object_(v1, v2, v3) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4) : object_(v1, v2, v3, v4) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4, const Arg5& v5) : object_(v1, v2, v3, v4, v5) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4, const Arg5& v5, const Arg6& v6) : object_(v1, v2, v3, v4, v5, v6) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4, const Arg5& v5, const Arg6& v6, const Arg7& v7) : object_(v1, v2, v3, v4, v5, v6, v7) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4, const Arg5& v5, const Arg6& v6, const Arg7& v7, const Arg8& v8) : object_(v1, v2, v3, v4, v5, v6, v7, v8) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4, const Arg5& v5, const Arg6& v6, const Arg7& v7, const Arg8& v8, const Arg9& v9) : object_(v1, v2, v3, v4, v5, v6, v7, v8, v9) {}

#endif

		virtual const std::type_info& type()
		{
			return metatableType<T>();
		}

		virtual void* get()
		{
			return &object_;
		}
		virtual const void* cget()
		{
			return &object_;
		}
	private:
		T object_;
	};


	struct ObjectSharedPointerWrapper : ObjectWrapperBase
	{
		template<typename T>
		ObjectSharedPointerWrapper(const standard::shared_ptr<T>& sptr) :object_(standard::const_pointer_cast<typename standard::remove_const<T>::type>(sptr)), type_(metatableType<T>()),
			shared_ptr_type_(metatableType<standard::shared_ptr<typename traits::decay<T>::type> >()), const_value_(traits::is_const<T>::value) {}
#if KAGUYA_USE_RVALUE_REFERENCE
		template<typename T>
		ObjectSharedPointerWrapper(standard::shared_ptr<T>&& sptr) : object_(std::move(standard::const_pointer_cast<typename standard::remove_const<T>::type>(sptr))), type_(metatableType<T>()),
			shared_ptr_type_(metatableType<standard::shared_ptr<typename traits::decay<T>::type> >()), const_value_(traits::is_const<T>::value){}
#endif
		virtual const std::type_info& type()
		{
			return type_;
		}
		virtual void* get()
		{
			return  const_value_ ? 0 : object_.get();
		}
		virtual const void* cget()
		{
			return object_.get();
		}
		standard::shared_ptr<void> object()const { return const_value_? standard::shared_ptr<void>():object_; }
		standard::shared_ptr<const void> const_object()const { return object_; }
		const std::type_info& shared_ptr_type()const { return shared_ptr_type_; }


		virtual const std::type_info& native_type()
		{
			return metatableType<standard::shared_ptr<void> >();
		}
		virtual void* native_get() { return &object_; }
	private:
		standard::shared_ptr<void> object_;
		const std::type_info& type_;

		const std::type_info& shared_ptr_type_;
		bool const_value_;
	};

	template<typename T, typename ElementType = typename T::element_type>
	struct ObjectSmartPointerWrapper : ObjectWrapperBase
	{
		ObjectSmartPointerWrapper(const T& sptr) :object_(sptr) {}
#if KAGUYA_USE_RVALUE_REFERENCE
		ObjectSmartPointerWrapper(T&& sptr) : object_(std::move(sptr)) {}
#endif
		virtual const std::type_info& type()
		{
			return metatableType<ElementType>();
		}
		virtual void* get()
		{
			return object_ ? &(*object_) : 0;
		}
		virtual const void* cget()
		{
			return object_ ? &(*object_) : 0;
		}
		virtual const std::type_info& native_type() 
		{
			return metatableType<T>();
		}
		virtual void* native_get() { return &object_; }
	private:
		T object_;
	};

	template<class T>
	struct ObjectPointerWrapper : ObjectWrapperBase
	{
		ObjectPointerWrapper(T* ptr) :object_(ptr) {}

		virtual const std::type_info& type()
		{
			return metatableType<T>();
		}
		virtual void* get()
		{
			if (traits::is_const<T>::value)
			{
				return 0;
			}
			return const_cast<void*>(static_cast<const void*>(object_));
		}
		virtual const void* cget()
		{
			return object_;
		}
		~ObjectPointerWrapper()
		{
		}
	private:
		T* object_;
	};

	//for internal use
	struct PointerConverter
	{
		template<typename T, typename F>
		static void* base_pointer_cast(void* from)
		{
			return static_cast<T*>(static_cast<F*>(from));
		}
		template<typename T, typename F>
		static standard::shared_ptr<void> base_shared_pointer_cast(const standard::shared_ptr<void>& from)
		{
			return standard::shared_ptr<T>(standard::static_pointer_cast<F>(from));
		}


		typedef void* (*convert_function_type)(void*);
		typedef standard::shared_ptr<void>(*shared_ptr_convert_function_type)(const standard::shared_ptr<void>&);
		typedef std::pair<std::string, std::string> convert_map_key;


		template<typename ToType, typename FromType>
		void add_type_conversion()
		{
			add_function(metatableType<ToType>(), metatableType<FromType>(), &base_pointer_cast<ToType, FromType>);
			add_function(metatableType<standard::shared_ptr<ToType> >(), metatableType<standard::shared_ptr<FromType> >(), &base_shared_pointer_cast<ToType, FromType>);
		}


		template<typename TO>
		TO* get_pointer(ObjectWrapperBase* from)const
		{
			const std::type_info& to_type = metatableType<TO>();
			//unreachable
			//if (to_type == from->type())
			//{
			//	return static_cast<TO*>(from->get());
			//}
			std::map<convert_map_key, std::vector<convert_function_type> >::const_iterator match = function_map_.find(convert_map_key(to_type.name(), from->type().name()));
			if (match != function_map_.end())
			{
				return static_cast<TO*>(pcvt_list_apply(from->get(), match->second));
			}
			return 0;

		}
		template<typename TO>
		const TO* get_const_pointer(ObjectWrapperBase* from)const
		{
			const std::type_info& to_type = metatableType<TO>();
			//unreachable
			//if (to_type == from->type())
			//{
			//	return static_cast<const TO*>(from->cget());
			//}
			std::map<convert_map_key, std::vector<convert_function_type> >::const_iterator match = function_map_.find(convert_map_key(to_type.name(), from->type().name()));
			if (match != function_map_.end())
			{
				return static_cast<const TO*>(pcvt_list_apply(const_cast<void*>(from->cget()), match->second));
			}
			return 0;
		}

		template<typename TO>
		standard::shared_ptr<TO> get_shared_pointer(ObjectSharedPointerWrapper* from)const
		{
			const std::type_info& to_type = metatableType<standard::shared_ptr<typename traits::decay<TO>::type> >();
			//unreachable
//			if (to_type == from->type())
//			{
//				return standard::static_pointer_cast<TO>(from->object());
//			}
			const std::type_info& from_type = from->shared_ptr_type();
			std::map<convert_map_key, std::vector<shared_ptr_convert_function_type> >::const_iterator match = shared_ptr_function_map_.find(convert_map_key(to_type.name(), from_type.name()));
			if (match != shared_ptr_function_map_.end())
			{
				standard::shared_ptr<void> sptr = from->object();

				if (!sptr && standard::is_const<TO>::value)
				{
					sptr = standard::const_pointer_cast<void>(from->const_object());
				}

				return standard::static_pointer_cast<TO>(pcvt_list_apply(sptr, match->second));
			}
			return standard::shared_ptr<TO>();
		}

		template<class T>
		T* get_pointer(ObjectWrapperBase* from, types::typetag<T> tag)
		{
			return get_pointer<T>(from);
		}
		template<class T>
		standard::shared_ptr<T> get_pointer(ObjectWrapperBase* from, types::typetag<standard::shared_ptr<T> > tag)
		{
			ObjectSharedPointerWrapper* ptr = dynamic_cast<ObjectSharedPointerWrapper*>(from);
			if (ptr)
			{
				return get_shared_pointer<T>(ptr);
			}
			return standard::shared_ptr<T>();
		}


		static int deleter(lua_State *state)
		{
			PointerConverter* ptr = (PointerConverter*)lua_touserdata(state, 1);
			ptr->~PointerConverter();
			return 0;
		}

		static PointerConverter& get(lua_State* state)
		{
			static char kaguya_ptrcvt_key_ptr;
			util::ScopedSavedStack save(state);
			lua_pushlightuserdata(state, &kaguya_ptrcvt_key_ptr);
			lua_gettable(state, LUA_REGISTRYINDEX);
			if (lua_isuserdata(state, -1))
			{
				return *static_cast<PointerConverter*>(lua_touserdata(state, -1));
			}
			else
			{
				void* ptr = lua_newuserdata(state, sizeof(PointerConverter));//dummy data for gc call
				PointerConverter* converter = new(ptr) PointerConverter();

				lua_createtable(state, 0, 0);
				lua_pushcclosure(state, &deleter, 0);
				lua_setfield(state, -2, "__gc");
				lua_pushvalue(state, -1);
				lua_setfield(state, -2, "__index");
				lua_setmetatable(state, -2);//set to userdata
				lua_pushlightuserdata(state, &kaguya_ptrcvt_key_ptr);
				lua_pushvalue(state, -2);
				lua_settable(state, LUA_REGISTRYINDEX);
				return *converter;
			}
		}
	private:
		void add_function(const std::type_info& to_type, const std::type_info& from_type, convert_function_type f)
		{
			std::map<convert_map_key, std::vector<convert_function_type> > add_map;
			for (std::map<convert_map_key, std::vector<convert_function_type> >::iterator it = function_map_.begin();
				it != function_map_.end(); ++it)
			{
				if (it->first.second == to_type.name())
				{
					std::vector<convert_function_type> newlist;
					newlist.push_back(f);
					newlist.insert(newlist.end(), it->second.begin(), it->second.end());
					add_map[convert_map_key(it->first.first, from_type.name())] = newlist;
				}
			}
			function_map_.insert(add_map.begin(), add_map.end());

			std::vector<convert_function_type> flist; flist.push_back(f);
			function_map_[convert_map_key(to_type.name(), from_type.name())] = flist;
		}
		void add_function(const std::type_info& to_type, const std::type_info& from_type, shared_ptr_convert_function_type f)
		{
			std::map<convert_map_key, std::vector<shared_ptr_convert_function_type> > add_map;
			for (std::map<convert_map_key, std::vector<shared_ptr_convert_function_type> >::iterator it = shared_ptr_function_map_.begin();
				it != shared_ptr_function_map_.end(); ++it)
			{
				if (it->first.second == to_type.name())
				{
					std::vector<shared_ptr_convert_function_type> newlist;
					newlist.push_back(f);
					newlist.insert(newlist.end(), it->second.begin(), it->second.end());
					add_map[convert_map_key(it->first.first, from_type.name())] = newlist;
				}
			}
			shared_ptr_function_map_.insert(add_map.begin(), add_map.end());

			std::vector<shared_ptr_convert_function_type> flist; flist.push_back(f);
			shared_ptr_function_map_[convert_map_key(to_type.name(), from_type.name())] = flist;
		}

		void* pcvt_list_apply(void* ptr, const std::vector<convert_function_type>& flist)const
		{
			for (std::vector<convert_function_type>::const_iterator i = flist.begin(); i != flist.end(); ++i)
			{
				ptr = (*i)(ptr);
			}
			return ptr;
		}
		standard::shared_ptr<void> pcvt_list_apply(standard::shared_ptr<void> ptr, const std::vector<shared_ptr_convert_function_type>& flist)const
		{
			for (std::vector<shared_ptr_convert_function_type>::const_iterator i = flist.begin(); i != flist.end(); ++i)
			{

#if KAGUYA_USE_CPP11
				ptr = (*i)(std::move(ptr));
#else
				ptr = (*i)(ptr);
#endif
			}
			return ptr;
		}


		PointerConverter() {}

		std::map<convert_map_key, std::vector<convert_function_type> > function_map_;
		std::map<convert_map_key, std::vector<shared_ptr_convert_function_type> > shared_ptr_function_map_;



		PointerConverter(PointerConverter&);
		PointerConverter& operator=(PointerConverter&);
	};

	namespace detail
	{
		inline bool object_wrapper_type_check(lua_State* l, int index)
		{
#if KAGUYA_NO_USERDATA_TYPE_CHECK
			return lua_isuserdata(l, index) && !lua_islightuserdata(l, index);
#endif
			if (lua_getmetatable(l, index))
			{
				int type = lua_rawgetp_rtype(l, -1, metatable_name_key());
				lua_pop(l, 2);
				return type == LUA_TSTRING;
			}
			return false;
		}
	}

	inline ObjectWrapperBase* object_wrapper(lua_State* l, int index)
	{
		if (detail::object_wrapper_type_check(l, index))
		{
			ObjectWrapperBase* ptr = static_cast<ObjectWrapperBase*>(lua_touserdata(l, index));
			return ptr;
		}
		return 0;
	}

	template<typename RequireType>
	inline ObjectWrapperBase* object_wrapper(lua_State* l, int index, bool convert = true, types::typetag<RequireType> type = types::typetag<RequireType>())
	{
		if (detail::object_wrapper_type_check(l, index))
		{
			ObjectWrapperBase* ptr = static_cast<ObjectWrapperBase*>(lua_touserdata(l, index));

			if (ptr->type() == metatableType<RequireType>())
			{
				return ptr;
			}
			else if (convert)
			{
				PointerConverter& pcvt = PointerConverter::get(l);
				return pcvt.get_pointer(ptr, types::typetag<RequireType>()) ? ptr : 0;
			}
			return 0;
		}
		return 0;
	}

	template<class T>
	T* get_pointer(lua_State* l, int index, types::typetag<T> tag)
	{
		int type = lua_type(l, index);

		if (type == LUA_TLIGHTUSERDATA)
		{
			return (T*)lua_topointer(l, index);
		}
		else if (type != LUA_TUSERDATA)
		{
			return 0;
		}
		else
		{
			ObjectWrapperBase* objwrapper = object_wrapper(l, index);
			if (objwrapper)
			{
				const std::type_info& to_type = metatableType<T>();
				if (objwrapper->type() == to_type)
				{
					return static_cast<T*>(objwrapper->get());
				}
				if (objwrapper->native_type() == to_type)
				{
					return static_cast<T*>(objwrapper->native_get());
				}
				else
				{
					PointerConverter& pcvt = PointerConverter::get(l);
					return pcvt.get_pointer<T>(objwrapper);
				}
			}
		}
		return 0;
	}
	template<class T>
	const T* get_const_pointer(lua_State* l, int index, types::typetag<T> tag)
	{
		int type = lua_type(l, index);

		if (type == LUA_TLIGHTUSERDATA)
		{
			return (T*)lua_topointer(l, index);
		}
		else if (type != LUA_TUSERDATA)
		{
			return 0;
		}
		else
		{
			ObjectWrapperBase* objwrapper = object_wrapper(l, index);
			if (objwrapper)
			{
				if (objwrapper->type() == metatableType<T>())
				{
					return static_cast<const T*>(objwrapper->cget());
				}
				else
				{
					PointerConverter& pcvt = PointerConverter::get(l);
					return pcvt.get_const_pointer<T>(objwrapper);
				}
			}
		}
		return 0;
	}
	template<class T>
	const T* get_pointer(lua_State* l, int index, types::typetag<const T> tag)
	{
		return get_const_pointer<T>(l,index, types::typetag<T>());
	}
	
	template<class T>
	standard::shared_ptr<T> get_shared_pointer(lua_State* l, int index, types::typetag<T> tag)
	{
		ObjectSharedPointerWrapper* ptr = dynamic_cast<ObjectSharedPointerWrapper*>(object_wrapper(l, index));
		if (ptr)
		{
			const std::type_info& from_type = ptr->shared_ptr_type();
			const std::type_info& to_type = metatableType<standard::shared_ptr<typename traits::decay<T>::type> >();
			if (from_type == to_type)
			{
				if (standard::is_const<T>::value)
				{
					return standard::static_pointer_cast<T>(standard::const_pointer_cast<void>(ptr->const_object()));
				}
				else
				{
					return standard::static_pointer_cast<T>(ptr->object());
				}
			}
			PointerConverter& pcvt = PointerConverter::get(l);
			return pcvt.get_shared_pointer<T>(ptr);
		}
		return standard::shared_ptr<T>();
	}
	inline standard::shared_ptr<void> get_shared_pointer(lua_State* l, int index, types::typetag<void> tag)
	{
		ObjectSharedPointerWrapper* ptr = dynamic_cast<ObjectSharedPointerWrapper*>(object_wrapper(l, index));
		if (ptr) 
		{
			return ptr->object();
		}
		return standard::shared_ptr<void>();
	}
	inline standard::shared_ptr<const void> get_shared_pointer(lua_State* l, int index, types::typetag<const void> tag)
	{
		ObjectSharedPointerWrapper* ptr = dynamic_cast<ObjectSharedPointerWrapper*>(object_wrapper(l, index));
		if (ptr)
		{
			return ptr->const_object();
		}
		return standard::shared_ptr<const void>();
	}

	namespace class_userdata
	{
		template<typename T>inline void destructor(T* pointer)
		{
			if (pointer)
			{
				pointer->~T();
			}
		}
		template<typename T>bool get_metatable(lua_State* l)
		{
			int ttype = lua_rawgetp_rtype(l, LUA_REGISTRYINDEX, metatable_type_table_key());//get metatable registry table
			if (ttype != LUA_TTABLE)
			{
				lua_newtable(l);
				lua_rawsetp(l, LUA_REGISTRYINDEX, metatable_type_table_key());
				return false;
			}
			int type = lua_rawgetp_rtype(l, -1, &metatableType<T>());
			lua_remove(l, -2);//remove metatable registry table
			return type != LUA_TNIL;
		}
		template<typename T>bool available_metatable(lua_State* l)
		{
			util::ScopedSavedStack save(l);
			return get_metatable<T>(l);
		}

		template<typename T>bool newmetatable(lua_State* l)
		{
			if (get_metatable<T>(l)) //already register
			{
				return false;  //
			}
			lua_pop(l, 1);
			lua_rawgetp_rtype(l, LUA_REGISTRYINDEX, metatable_type_table_key());//get metatable registry table
			int metaregindex = lua_absindex(l, -1);

			lua_createtable(l, 0, 2);
			lua_pushstring(l, metatableName<T>());
			lua_pushvalue(l,-1);
			lua_setfield(l, -3, "__name");  // metatable.__name = name
			lua_rawsetp(l, -2, metatable_name_key());
			lua_pushvalue(l, -1);
			lua_rawsetp(l, metaregindex, &metatableType<T>());
			lua_remove(l, metaregindex);//remove metatable registry table

			return true;
		}


		template<typename T>inline int deleter(lua_State *state)
		{
			T* ptr = (T*)lua_touserdata(state, 1);
			ptr->~T();
			return 0;
		}
		struct UnknownType {};
		template<typename T>void setmetatable(lua_State* l)
		{
			//if not available metatable, set unknown class metatable
			if (!get_metatable<T>(l))
			{
				lua_pop(l, 1);
				if (!get_metatable<UnknownType>(l))
				{
					lua_pop(l, 1);
					newmetatable<UnknownType>(l);
					lua_pushcclosure(l, &deleter<ObjectWrapperBase>, 0);
					lua_setfield(l, -2, "__gc");
				}
			}
			lua_setmetatable(l, -2);
		}
	}
	template<typename T>
	bool available_metatable(lua_State* l, types::typetag<T> type = types::typetag<T>())
	{
		return class_userdata::available_metatable<T>(l);
	}
}
