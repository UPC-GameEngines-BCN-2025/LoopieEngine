#pragma once

#include <filesystem>

#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;




namespace Loopie {

    template <typename T>
    struct JsonResult {
        T Result{};
        bool Found = false;
    };

    class JsonNode {
    public:
        JsonNode() = default;
        JsonNode(json* node);
        JsonNode(json* node, json* parentNode);

        bool IsValid() const { return m_node != nullptr; }
        bool HasParent() const { return m_parentNode != nullptr; }
        bool IsObject() const { return IsValid() && m_node->is_object(); }
        bool IsArray() const { return IsValid() && m_node->is_array(); }
        bool IsEmpty() const { return Size() == 0; }

        unsigned int Size(const std::string& keyPath);
        unsigned int Size() const { return IsValid() ? (unsigned int)m_node->size() : 0; }

        bool IsArrayEmpty(const std::string& keyPath) const;
        bool IsArrayEmpty() const { return  Size() == 0 ? true : false; }

        

        JsonNode Child(const std::string& keyPath) const;
        bool Contains(const std::string& keyPath) const;

        template <typename T>
        JsonResult<T> GetValue(const std::string& keyPath, T defaultValue = {}) const {
            if (keyPath.empty())
                return GetValue<T>(defaultValue);

            JsonNode node = Child(keyPath);
            if (node.IsValid()) {
                return node.GetValue(defaultValue);
            }
            JsonResult<T> result;
            result.Found = false;
            return result;
        }

        template <typename T>
        JsonResult<T> GetValue(T defaultValue = {}) const {
            JsonResult<T> result;
            if (IsValid()) {
                result.Result = m_node->get<T>();
                result.Found = true;
            }
            else {
                result.Found = false;
            }
            return result;
        }

        template <typename T>
        bool SetValue(const std::string& keyPath, T value) {
            if (keyPath.empty())
                return SetValue<T>(value);

            JsonNode node = Child(keyPath);
            if (node.IsValid()) {
                node.SetValue(value);
                return true;
            }
            return false;
        }

        template <typename T>
        bool SetValue(T value) {
            if (IsValid()) {
                *m_node = value;
                return true;
            }
            return false;
        }

        std::vector<std::string> GetObjectKeys(const std::string& keyPath) const;
        std::vector<std::string> GetObjectKeys() const;

        bool HasKey(const std::string& keyPath, const std::string& key) const;
        bool HasKey(const std::string& key) const;


        JsonNode CreateObjectField(const std::string& keyPath);
        JsonNode CreateArrayField(const std::string& keyPath);

        template <typename T>
        JsonNode CreateField(const std::string& keyPath, T value) {
            if (!IsValid() || keyPath.empty()) return JsonNode();

            auto pos = keyPath.find_last_of('.');
            std::string parentPath = (pos == std::string::npos) ? "" : keyPath.substr(0, pos);
            std::string key = (pos == std::string::npos) ? keyPath : keyPath.substr(pos + 1);

            JsonNode parent = parentPath.empty() ? *this : Child(parentPath);
            if (!parent.IsObject()) return JsonNode();

            (*parent.m_node)[key] = value;
            return JsonNode(&(*parent.m_node)[key]);
        }

        template <typename T>
        JsonResult<T> GetArrayElement(const std::string& keyPath, unsigned int index) {
            if (keyPath.empty())
                return GetArrayElement<T>(index);

            JsonNode node = Child(keyPath);
            if (node.IsArray()) return node.GetArrayElement<T>(index);
            
            JsonResult<T> result;
            result.Found = false;
            return result;
        }

        template <typename T>
        JsonResult<T> GetArrayElement(unsigned int index) {
            JsonResult<T> result;
            if (IsArray()) {
                unsigned int size = Size();
                if (size == 0 || size <= index) {
                    result.Found = false;
                    return result;
                }
                result.Found = true;
                result.Result = (*m_node)[index].get<T>();
                return result;
            }
            result.Found = false;
            return result;
        }

        template <typename T>
        bool AddArrayElement(const std::string& keyPath, T value) {
            if (keyPath.empty())
                return AddArrayElement(value);

            JsonNode node = Child(keyPath);
            if (!node.IsArray()) return false;

            node.m_node->push_back(value);
            return true;
        }

        template <typename T>
        bool AddArrayElement(T value) {
            if (!IsArray())
                return false;

            m_node->push_back(value);
            return true;
        }

        template <typename T>
        bool ModifyArrayElement(const std::string& keyPath, unsigned int index, T value) {
            if (keyPath.empty())
                return ModifyArrayElement(index, value);

            JsonNode node = Child(keyPath);
            if (!node.IsArray())
                return false;

            unsigned int size = node.Size();
            if (size == 0 || size <= index)
                return false;

            (node.m_node->begin() + index).value() = value;
            return true;
        }

        template <typename T>
        bool ModifyArrayElement(unsigned int index, T value) {
            if (!IsArray())
                return false;

            unsigned int size = Size();
            if (size == 0 || size <= index)
                return false;

            (m_node->begin() + index).value() = value;
            return true;
        }

        bool Remove(const std::string& keyPath);
        bool RemoveSelf(const std::string& name);
        bool RemoveArrayElement(const std::string& keyPath, unsigned int index);
        bool RemoveArrayElement(unsigned int index);

        bool ClearArray(const std::string& keyPath);
        bool ClearArray();

        std::string ToString(int indent = 4) const;

    private:
            void Reset();

    private:
        json* m_node = nullptr;
        json* m_parentNode = nullptr;
    };


    class JsonData {
    public:
        friend class JsonNode;
        friend class Json;

        bool IsEmpty() const { return m_empty; }

        unsigned int Size() {
            return Root().Size();
        }

        JsonNode Child(const std::string& keyPath) {
            return Root().Child(keyPath);
        }

        template <typename T>
        JsonResult<T> GetValue(const std::string& keyPath, T defaultValue = {}) {
            return Root().GetValue(keyPath, defaultValue);
        }

        template <typename T>
        bool SetValue(const std::string& keyPath, T value) {
            return Root().SetValue(keyPath, value);
        }

        std::vector<std::string> GetObjectKeys(const std::string& keyPath) {
            return Root().GetObjectKeys(keyPath);
        }

        bool HasKey(const std::string& keyPath, const std::string& key) {
            return Root().HasKey(keyPath, key);
        }

        template <typename T>
        bool CreateField(const std::string& keyPath, T value) {
            return Root().CreateField(keyPath, value).IsValid();
        }

        JsonNode CreateObjectField(const std::string& keyPath) {
            return Root().CreateObjectField(keyPath);
        }

        JsonNode CreateArrayField(const std::string& keyPath) {
            return Root().CreateArrayField(keyPath);
        }

        template <typename T>
        JsonResult<T> GetArrayElement(const std::string& keyPath, unsigned int index) {
            return Root().GetArrayElement<T>(keyPath, index);
        }

        template <typename T>
        bool AddArrayElement(const std::string& keyPath, T value) {
            return Root().AddArrayElement(keyPath, value);
        }

        template <typename T>
        bool ModifyArrayElement(const std::string& keyPath, unsigned int index, T value) {
            return Root().ModifyArrayElement(keyPath, index, value);
        }

        bool RemoveArrayElement(const std::string& keyPath, unsigned int index) {
            return Root().RemoveArrayElement(keyPath, index);
        }

        bool ClearArray(const std::string& keyPath) {
            return Root().ClearArray(keyPath);
        }

        bool Remove(const std::string& keyPath) {
            return Root().Remove(keyPath);
        }

        std::string ToString(int indent = 4) const {
            return m_data.dump(indent);
        }

        JsonNode Root() { return JsonNode(&m_data); }

        bool ToFile(const std::filesystem::path& filePath, int indent = 4);

    private:
        json m_data = json::object();
        bool m_empty = true;
    };


    class Json {
    public:
        static JsonData ReadFromString(const std::string& data);
        static JsonData ReadFromFile(const std::filesystem::path& filePath);
        static bool WriteToFileFromData(const std::filesystem::path& filePath, const JsonData& jsonString, int indent = 4);
        static bool WriteToFileFromString(const std::filesystem::path& filePath, const std::string& jsonData, int indent = 4);
    };
}