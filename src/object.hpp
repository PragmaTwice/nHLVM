#pragma once

#include <memory>
#include <vector>
#include <type_traits>
#include <typeinfo>
#include <functional>
#include <string>
#include <deferred_allocator.h>

namespace nhlvm
{
	class object;
	using object_ptr = std::shared_ptr<object>;

	template <typename>
	class object_holder;

	template <typename>
	class object_specialize;

	inline namespace object_base
	{
		using basic_int    = long long;
		using basic_float  = long double;
		using basic_bool   = bool;
		using basic_string = std::string;
		using basic_func   = std::function<object_ptr(object_ptr)>;
		using basic_array  = std::vector<object_ptr>;

	}

	using object_int    = object_specialize<basic_int>;
	using object_float  = object_specialize<basic_float>;
	using object_bool   = object_specialize<basic_bool>;
	using object_string = object_specialize<basic_string>;
	using object_func   = object_specialize<basic_func>;
	using object_array  = object_specialize<basic_array>;


	// RAII info interface
	class object
	{
	public:

		virtual object_ptr description() const = 0;
		virtual object_ptr methods() const = 0;

		virtual ~object() = 0;

	};

	// dummy implementation of virtual destructor
	object::~object() {}

	// data holder
	template <typename T>
	class object_holder : public object, protected std::shared_ptr<T>
	{
	public:

		template <typename U, typename ...Args>
		explicit object_holder(std::in_place_type_t<U>, Args && ...args) : std::shared_ptr<T>(new U{ std::forward<Args>(args)... }) {
			static_cast(std::is_same_v<T,U> || (std::is_same_v<T,object> && std::is_base_of_v<T,U>), "object_holder in_place constructor : invaild template argument");
		}

		object_holder(const std::shared_ptr<T>& src) : std::shared_ptr<T>(src) {}
		object_holder(std::shared_ptr<T>&& src) : std::shared_ptr<T>(src) {}

		object_holder(const object_holder &) = default;
		object_holder(object_holder &&) = default;

		object_holder& operator=(const object_holder& src){
			return (object_holder&)this->std::shared_ptr<T>::operator=(src);
		}
		object_holder& operator=(object_holder&& src){
			return (object_holder&)this->std::shared_ptr<T>::operator=(src);
		}

		object_ptr description() const {
			return { new object_int{ typeid(this->std::shared_ptr<T>::operator*()).hash_code() } };
		}

	};

	// deduction guide for object_holder in_place constructor
	/* // fuck MSVC 2017 15.4 because of no support of deduction guide
	template <typename U, typename ...Args>
	object_holder(std::in_place_type_t<U>, Args && ...args) -> object_holder<U>;
	*/

	// specialization for object_holder
	template <>
	class object_specialize<basic_int> : public object_holder<basic_int>
	{
	public:

		using object_holder<basic_int>::object_holder;

	};

	template <>
	class object_specialize<basic_float> : public object_holder<basic_float>
	{
	public:

		using object_holder<basic_float>::object_holder;

	};

	template <>
	class object_specialize<basic_bool> : public object_holder<basic_bool>
	{
	public:

		using object_holder<basic_bool>::object_holder;

	};

	template <>
	class object_specialize<basic_string> : public object_holder<basic_string>
	{
	public:

		using object_holder<basic_string>::object_holder;

	};

	template <>
	class object_specialize<basic_func> : public object_holder<basic_func>
	{
	public:

		using object_holder<basic_func>::object_holder;

	};

	template <>
	class object_specialize<basic_array> : public object_holder<basic_array>
	{
	public:

		using object_holder<basic_array>::object_holder;

	};

}
