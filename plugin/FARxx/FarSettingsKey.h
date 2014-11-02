#pragma once
#include <memory>
#include <plugin.hpp>
#include <string>
#include <PluginSettings.hpp>

#include "FarGlobal.h"


// forwards
class PluginSettings;
class FarSettingsKey;
class FarSettingsRoot;

class FarSettingsKey
{
public:
    class Root
    {
        typedef std::weak_ptr<PluginSettings>   TSettingsCache;

    public:
        typedef std::shared_ptr<PluginSettings> TSettingsPtr;

        static FarSettingsKey get(const std::wstring& name, const GUID& guid = FarGlobal::instance()->mainGuid());
    };

private:
    template <typename T>
    struct traits
    {
        enum 
        {
            IsIntegral = std::is_integral<typename T>::value,
            IsArray    = std::is_array<typename T>::value,
            IsPodClass = std::is_pod<typename T>::value && std::is_class<typename T>::value,
            IsWstring  = std::is_same<typename T, std::wstring>::value,
        };
    };

    template <typename T> 
    static typename std::enable_if<traits<T>::IsIntegral, FARSETTINGSTYPES>::type TypeToFST() { return FST_QWORD; }

    template <typename T> 
    static typename std::enable_if<traits<T>::IsWstring, FARSETTINGSTYPES>::type TypeToFST() { return FST_STRING; }


    FarSettingsKey(const Root::TSettingsPtr& settings, size_t root, const std::wstring& name);

    Root::TSettingsPtr m_settings;
    size_t             m_root;
    std::wstring       m_name;

public:
    ~FarSettingsKey();


    template <typename Integral>
    typename std::enable_if<traits<typename Integral>::IsIntegral, typename Integral>::type getNumber(uint64_t defValue=0) const
    {
        uint64_t value = m_settings->Get(m_root, m_name.c_str(), defValue);
        return static_cast<std::remove_cv<Integral>::type>(value);
    }

    template <typename Integral>
    typename std::enable_if<traits<typename Integral>::IsIntegral, void>::type setNumber(Integral value)
    {
        uint64_t qword = static_cast<uint64_t>(value);
        m_settings->Set(m_root, m_name.c_str(), qword);
    }

    std::wstring getWstring(const std::wstring& defValue = L"") const;
    void         setWstring(const std::wstring& value);

};



