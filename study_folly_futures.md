Stage: **then**

template <typename F, typename R = futures::detail::callableResult<T, F>>
typename R::Return then(F&& func) {
  return this->template thenImplementation<F, R>(
      std::forward<F>(func), typename R::Arg());
}

Note:
- callback funcion return future, conitnue to then, otherwise assign to a future var?

```C++

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

```
