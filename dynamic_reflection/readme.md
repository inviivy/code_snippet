# dynamic reflection

# Adapt Pattern
C++ struct <--Adaptee--> Json/XML/...字段映射

# pointer to member of pointe

+ [pointer to member of pointer](https://en.cppreference.com/w/cpp/language/operator_member_access)

```cpp
template<typename StructType, typename FieldType>
void func(FieldType StructType::* pointer) {
    // ...
    // 
}
```

# reference
+ [简单的 C++ 结构体字段反射](https://bot-man-jl.github.io/articles/?post=2018/Cpp-Struct-Field-Reflection)