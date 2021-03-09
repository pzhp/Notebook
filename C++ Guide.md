
# 1 Core C++

## 1) name lookup & ODR
```
two stage name lookup
```
## 2) virutal function
## 3) type deductiton
## 4) overload&conversion:
``` 
    explicit： static_cast
    implicit:
        standard conversion:
            lvalue-transformation
            pointer order
                derived => base (shortest path) / void* / bool / to pointer, other int cannot 
        user-defined conversion
```
## 5) instantiation:
    when to trigger instantiation(explicit & implicit)
    what to check
## 6) specialization   
## 7) misc
    lvalue&rvalue
    two phase initialization
    SFINAE
        enable_if/void_t/decltype/declval/consexpr
    

# 2 Advance
## lib:
    STL/BOOST/Loki
    Folly
    GMOCK/GTEST
    QT
        Dynamic: QMetaType/QMetaMethod/QMetaObject/Signal-slot
    
## code gen:
    macro
    template
    tool (moc/protobuf)

## pattern:
    structure
    OOP
    template
    functional
    generic program
    statful/type rich
## util:
    Debug/profile
    Compiler/Linker
    Memory layout
    Memory model: atomic & lock

# 3 Cases:
    auto_ptr/auto_ptr_ref
    c++ idiom



