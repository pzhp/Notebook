#include <unordered_map>
#include <vector>
#include <functional>
#include <iostream>

namespace cppt {
	
template<class...>
struct type_list{};

template<class... TYPES>
struct visitor_base
{
	using types = type_list<TYPES...>;
};

struct heterogeneous_container
{
    public:
    heterogeneous_container() = default;
    heterogeneous_container(const heterogeneous_container& _other)
    {
       *this = _other;
    }
 
    heterogeneous_container& operator=(const heterogeneous_container& _other)
    {
        clear();
        clear_functions = _other.clear_functions;
        copy_functions = _other.copy_functions;

        for (auto&& copy_function : copy_functions)
        {
            copy_function(_other, *this);
        }
        return *this;
    }
 
    template<class T>
    void push_back(const T& _t)
    {
        // don't have it yet, so create functions for copying and destroying
        if (items<T>.find(this) == std::end(items<T>))
        { 
            clear_functions.emplace_back([](heterogeneous_container& _c){items<T>.erase(&_c);});
 
            // if someone copies me, they need to call each copy_function and pass themself
            copy_functions.emplace_back([](const heterogeneous_container& _from, heterogeneous_container& _to)
            {
                items<T>[&_to] = items<T>[&_from];
            });
        }
        items<T>[this].push_back(_t);
    }
 
    void clear()
    {
        for (auto&& clear_func : clear_functions)
        {
            clear_func(*this);
        }
    }
 
    ~heterogeneous_container()
    {
        clear();
    } 

    template<class T, class U>
    void visit_impl_help(T& visitor)
    {
        for (auto&& element : items<U>[this])
        {
            visitor(element);
        }
    }

    template<class T, template<class...> class TLIST, class... TYPES>
    void visit_impl(T&& visitor, TLIST<TYPES...>) {
        (..., visit_impl_help<std::decay_t<T>, TYPES>(visitor));
    }

    template<class T>
    void visit(T&& visitor)
    {
        visit_impl(visitor, typename std::decay_t<T>::types{});
    }
 
    private:
    template<class T>
    static std::unordered_map<const heterogeneous_container*, std::vector<T>> items;
 
    std::vector<std::function<void(heterogeneous_container&)>> clear_functions;
    std::vector<std::function<void(const heterogeneous_container&, heterogeneous_container&)>> copy_functions;

};

template<class T>
std::unordered_map<const heterogeneous_container*, std::vector<T>> heterogeneous_container::items;

}

template <typename ...T>
struct print_visitor : public cppt::visitor_base <T ...> {
	template <typename U>
	void operator()(U&& u) {
		std::cout << u << std::endl;
	}
};

int main() {
	cppt::heterogeneous_container c;
	c.push_back('a');
	c.push_back(1);
	c.push_back(2.0);
	c.push_back(3);
	c.push_back(std::string{"foo"}); 
	auto print_container = [](cppt::heterogeneous_container& _in){_in.visit(print_visitor<char, int, float, int, std::string>{}); std::cout << std::endl;};
	print_container(c);
}
