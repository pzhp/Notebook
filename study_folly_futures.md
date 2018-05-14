Stage: **then**

**How is then implement?**
- callableResult store info: isTry, Argument Type by std::conditional<> && decltype && declval
- use Try in inner callback

```C++

template <typename F, typename R = futures::detail::callableResult<T, F>>
typename R::Return then(F&& func) {
  return this->template thenImplementation<F, R>(
      std::forward<F>(func), typename R::Arg());
}

template <typename F, typename R, bool isTry, typename... Args>
typename std::enable_if<!R::ReturnsFuture::value, typename R::Return>::type
thenImplementation(F&& func, futures::detail::argResult<isTry, F, Args...>);

// set it in callback
auto tf2 = state.tryInvoke(t.template get<isTry, Args>()...);

// call callback
core->callback_(std::move(*core->result_));

// 
template <typename F, typename... Args>
using resultOf = decltype(std::declval<F>()(std::declval<Args>()...));

template<typename F, typename... Args>
struct callablewith {
	template <typename T, typename = decltype(declval<T>()(std::declval<Args>() ...)) >
	static constexpr std::true_type
	check(std::nullptr_t) { return true_type{}; }
	
	template<typename T>
	static constexpr std::false_type
	check(...) { return std::false_type{}; }
	
	typedef decltype(check<F>(nullptr)) type;
	static constexpr bool value = type::value;
};

template <typename T, typename F>
struct callableResult {
  typedef typename std::conditional<
    callableWith<F>::value,
    detail::argResult<false, F>,
    typename std::conditional<
      callableWith<F, T&&>::value,
      detail::argResult<false, F, T&&>,
      typename std::conditional<
        callableWith<F, T&>::value,
        detail::argResult<false, F, T&>,
        typename std::conditional<
          callableWith<F, Try<T>&&>::value,
          detail::argResult<true, F, Try<T>&&>,
          detail::argResult<true, F, Try<T>&>>::type>::type>::type>::type Arg;
  typedef isFutureOrSemiFuture<typename Arg::Result> ReturnsFuture;
  typedef Future<typename ReturnsFuture::Inner> Return;
};

// Try
template <bool isTry, typename R>
typename std::enable_if<isTry, R>::type get() {
return std::forward<R>(*this);
}

template <bool isTry, typename R>
typename std::enable_if<!isTry, R>::type get() {
return std::forward<R>(value());
}

```

**How is wait ?**


