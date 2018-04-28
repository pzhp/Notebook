Some important C++ usage from [redxdev/ECS](https://github.com/redxdev/ECS/blob/master/ECS.h).
# ECS & world
E: is not extension point 

C: extended by base class and tmeplate subclass 

S: extended by subclass 

world

# type is map key
<typeinfo>   type_info
<typeindex>  type_index

# allocate
```C++
using ComponentAllocator = std::allocator_traits<World::EntityAllocator>::template rebind_alloc<Internal::ComponentContainer<T>>;

ComponentAllocator alloc(world->getPrimaryAllocator());

Internal::ComponentContainer<T>* container = std::allocator_traits<ComponentAllocator>::allocate(alloc, 1);
std::allocator_traits<ComponentAllocator>::construct(alloc, container, T(args...));

std::allocator_traits<EntityAllocator>::destroy(entAlloc, ent);
std::allocator_traits<EntityAllocator>::deallocate(entAlloc, ent, 1);
```

# iterator and view
```C++
// lambda-based each, there's also an iterator-based each

struct iterator {
    bool operator!=() {
    }

    bool operator++() {
        // filter items not match the condition
    }

    bool operator*() {
    }
};

struct View {
    const Iterator& begin() const {
        return _begin();
    }

    const Iterator& end() const {
        return _end;
    }
    iterator _begin;
    iterator _end;
};

for(auto& v, views) {

}

template<typename... Types>
Internal::EntityComponentView<Types...> each(bool bIncludePendingDestroy = false)
{
    Internal::EntityComponentIterator<Types...> first(this, 0, false, bIncludePendingDestroy);
    Internal::EntityComponentIterator<Types...> last(this, getCount(), true, bIncludePendingDestroy);
    return Internal::EntityComponentView<Types...>(first, last);
}
```
# select item by types
```C++
// test every type recursily expan package
template<typename... types>
selectEntity<typess...>()

template<typename T>
bool has() const {}

template<typename T, typname U, typename ... Types>
bool has() const {
    return has<T>() && has<U, Types...>();
}
```
# entity and component
```C++
// old way
struct MyComponent : public IConpnent{};
std::unorder_map<type_index, IConpnent*> components;

// new way: abstract base class + template subclass 
// component and event both use template subclass, not inherit
std::unorder_map<type_index, ConpnentBase*> components;
```

# event
```C++
// type is event: 
template<typename T>
class EventSubscriber : public Internal::BaseEventSubscriber
{
public:
    virtual ~EventSubscriber() {}
    virtual void receive(World* world, const T& event) = 0;
};

world->emit<Events::OnComponentAssigned<T>>({ this, handle });

struct MyEvent
{
    int foo;
    float bar;
}

class MyEventSubscriber : public EventSubscriber<MyEvent>
{
public:
    virtual ~MyEventSubscriber() {}
    virtual void receive(const MyEvent& event) override
    {
        std::cout << "MyEvent was emitted!" << std::endl;
    }
}

// ...

MyEventSubscriber* mySubscriber = new MyEventSubscriber();
world->subscribe<MyEvent>(mySubscriber);
```

# backup
```C++
template<typename... Ts> void func(Ts... args){
    const int size = sizeof...(args) + 2;
    int res[size] = {1,args...,2};
    // since initializer lists guarantee sequencing, this can be used to
    // call a function on each element of a pack, in order:
    int dummy[sizeof...(Ts)] = { (std::cout << args, 0)... };
}


template<class... Mixins>
class X : public Mixins... {
 public:
    X(const Mixins&... mixins) : Mixins(mixins)... { }
};

template<class...X> void func(int arg) throw(X...)
{
 // ... throw different Xs in different situations
}

// pack expanse
template <class ...Args>
void expand(Args... args)
{
   finalexpand(printarg(args)...);
}

int arr[] = {(printarg(args), 0)...};

//整型序列的定义
template<int...>
struct IndexSeq{};

//继承方式，开始展开参数包
template<int N, int... Indexes>
struct MakeIndexes : MakeIndexes<N - 1, N - 1, Indexes...> {};

// 模板特化，终止展开参数包的条件
template<int... Indexes>
struct MakeIndexes<0, Indexes...>
{
    typedefIndexSeq<Indexes...> type;
};

int main()
{
    using T = MakeIndexes<3>::type;
    cout <<typeid(T).name() << endl;
    return 0;
}
```
