#include "stdafx.h"
#include "FarSettingsKey.h"
#include <PluginSettings.hpp>

FarSettingsKey FarSettingsKey::Root::get(const std::wstring& name, const GUID& guid /*= FarGlobal::instance()->mainGuid()*/ )
{
    static TSettingsCache cache;

    FarGlobal* i = FarGlobal::instance();
    TSettingsPtr settings = cache.lock();
        
    if (settings.get() == nullptr)
    {
        settings = std::make_shared<PluginSettings>(guid, i->api().SettingsControl);
        cache    = settings;
    }

    return FarSettingsKey(settings, 0, name);
}


FarSettingsKey::FarSettingsKey(const FarSettingsKey::Root::TSettingsPtr& settings, size_t root, const std::wstring& name)
    : m_settings(settings)
    , m_root    (root)
    , m_name    (name)
{
}


FarSettingsKey::~FarSettingsKey(void)
{
}

std::wstring FarSettingsKey::getWstring(const std::wstring& defValue /*= L""*/ ) const
{
    return m_settings->Get(m_root, m_name.c_str(), defValue.c_str());
}

void FarSettingsKey::setWstring( const std::wstring& value )
{
    m_settings->Set(m_root, m_name.c_str(), value.c_str());
}
