#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>

/* 使用std::function定义C++字段_名称 转型到其他格式的接口 */
template <typename FieldType>
using ValueConverter =
    std::function<void(FieldType *field, const std::string &name)>;

/* 定义C++的struct/class 字段转换的接口 */
template <typename StructType> struct FieldConverterBase {
  virtual ~FieldConverterBase() = default;
  virtual void operator()(StructType *obj) const = 0;
};

/* 定义struct/class 字段转换到对应类型的具体实现 */
template <typename StructType, typename FieldType>
struct FieldConverter : public FieldConverterBase<StructType> {
  FieldConverter(const std::string &name, FieldType StructType::*pointer,
                 ValueConverter<FieldType> converter)
      : m_field_name(name), m_field_pointer(pointer),
        m_value_converter(converter) {}

  void operator()(StructType *obj) const override {
    return m_value_converter(std::addressof(obj->*m_field_pointer), m_field_name);
  }

private:
  std::string m_field_name;
  // 结构体中指向字段的指针, 其实就是相对于this的偏移
  FieldType StructType::*m_field_pointer;
  // 转换function
  ValueConverter<FieldType> m_value_converter;
};

/* 一个struct中有很多FieldConverter */
template <typename StructType> struct StructValueConverter {
  // 需要手动注册所有的类型信息和对应类型的callback
  template <typename FieldType>
  void RegisterField(FieldType StructType::*field_pointer,
                     const std::string &field_name,
                     ValueConverter<FieldType> value_converter) {
    m_fields.push_back(std::make_unique<FieldConverter<StructType, FieldType>>(
        field_name, field_pointer, std::move(value_converter)));
  }

  void operator()(StructType *obj) const {
    for (const auto &field_converter : m_fields) {
      (*field_converter)(obj);
    }
  }

private:
  std::vector<std::unique_ptr<FieldConverterBase<StructType>>> m_fields;
};