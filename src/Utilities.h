#pragma once

#include <array>

namespace F4
{
	inline void ShakeCamera(float a_multiplier, RE::NiPoint3 a_origin, float a_duration, float a_strength)
	{
		using func_t = decltype(&ShakeCamera);
		static REL::Relocation<func_t> func{ REL::ID{ 758209, 2214730 } };
		func(a_multiplier, a_origin, a_duration, a_strength);
	}

	inline void ApplyImageSpaceModifier(RE::TESImageSpaceModifier* a_modifier, float a_strength, RE::NiAVObject* a_target)
	{
		using func_t = decltype(&ApplyImageSpaceModifier);
		static REL::Relocation<func_t> func{ REL::ID{ 179769, 2199906 } };
		func(a_modifier, a_strength, a_target);
	}

	inline REL::Relocation<RE::NiPoint3*> ptr_kCurrentWorldLoc{ REL::ID{ 599780, 2712916 } };
}

inline void _MESSAGE(const char* a_format, ...)
{
	char buffer[2048]{};
	va_list args;
	va_start(args, a_format);
	vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, a_format, args);
	va_end(args);
	REX::INFO("{}", buffer);
}

template <class T>
T SafeWrite64Function(std::uintptr_t a_address, T a_replacement)
{
	static_assert(sizeof(T) >= sizeof(std::uintptr_t));
	std::uintptr_t replacement{};
	std::memcpy(&replacement, &a_replacement, sizeof(replacement));
	const auto originalAddress = *reinterpret_cast<std::uintptr_t*>(a_address);
	REL::WriteSafeData(a_address, replacement);
	T original{};
	std::memcpy(&original, &originalAddress, sizeof(originalAddress));
	return original;
}

inline RE::ActorValueInfo* GetAVIFByEditorID(const std::string& a_editorID)
{
	for (auto* form : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::ActorValueInfo>()) {
		if (form && std::strcmp(form->formEditorID.c_str(), a_editorID.c_str()) == 0) {
			return form;
		}
	}
	return nullptr;
}

inline std::string SplitString(const std::string& a_value, const std::string& a_delimiter, std::string& a_remainder)
{
	const auto position = a_value.find(a_delimiter);
	if (position == std::string::npos) {
		a_remainder.clear();
		return a_value;
	}
	a_remainder = a_value.substr(position + a_delimiter.length());
	return a_value.substr(0, position);
}

inline bool Visit(RE::NiAVObject* a_object, const std::function<bool(RE::NiAVObject*)>& a_visitor)
{
	if (!a_object || a_visitor(a_object)) {
		return a_object != nullptr;
	}
	if (auto* node = a_object->IsNode()) {
		for (auto& child : node->children) {
			if (child && Visit(child.get(), a_visitor)) {
				return true;
			}
		}
	}
	return false;
}

inline bool CheckPA(RE::Actor* a_actor)
{
	return a_actor && a_actor->extraList && a_actor->extraList->HasType(RE::EXTRA_DATA_TYPE::kPowerArmor);
}

inline RE::bhkNPCollisionObject* GetCollisionObject(RE::HitData& a_hitData)
{
	return reinterpret_cast<RE::NiPointer<RE::bhkNPCollisionObject>*>(
		reinterpret_cast<std::uintptr_t>(std::addressof(a_hitData.impactData)) + 0x30)->get();
}

inline void SetAllDamageToZero(RE::HitData& a_hitData)
{
	a_hitData.flags = static_cast<RE::HitData::Flag>(a_hitData.flags.underlying() & 0xFFFFFE07);
	a_hitData.healthDamage = 0.0F;
	a_hitData.totalDamage = 0.0F;
	a_hitData.physicalDamage = 0.0F;
	a_hitData.targetedLimbDamage = 0.0F;
	a_hitData.resistedPhysicalDamage = 0.0F;
	a_hitData.resistedTypedDamage = 0.0F;
}

inline bool CastSpell(RE::SpellItem* a_spell, RE::TESObjectREFR* a_caster, RE::TESObjectREFR* a_target, RE::Actor* a_actor)
{
	if (!a_spell || !a_caster) {
		return false;
	}

	auto* caster = a_caster->GetMagicCaster(static_cast<RE::MagicSystem::CastingSource>(3));
	if (!caster) {
		return false;
	}

	using interrupt_t = void (*)(RE::MagicCaster*, bool);
	static REL::Relocation<interrupt_t> interrupt{ REL::ID{ 788485, 2226294 } };
	interrupt(caster, false);

	using cast_t = void (*)(RE::MagicCaster*, RE::MagicItem*, bool, RE::TESObjectREFR*, float, bool, float, RE::Actor*);
	auto** vtable = *reinterpret_cast<std::uintptr_t***>(caster);
	auto cast = reinterpret_cast<cast_t>(vtable[2]);
	cast(caster, a_spell, false, a_target, 1.0F, false, 0.0F, a_actor);
	return true;
}

inline void PostModifyInventoryItemMod(RE::TESObjectREFR* a_container, RE::TESBoundObject* a_item, bool a_reEquip)
{
	using func_t = void (*)(RE::TESObjectREFR*, RE::TESBoundObject*, bool);
	static REL::Relocation<func_t> func{ REL::ID{ 1153963, 2254271 } };
	func(a_container, a_item, a_reEquip);
}

inline void SetupFilter(RE::bhkPickData& a_pick, RE::Actor* a_actor, RE::BGSProjectile* a_projectile, bool a_excludeActor)
{
	std::uint64_t excludedLayers = 0x1C15160;
	auto* collisionLayer = a_projectile ? a_projectile->data.collisionLayer : nullptr;
	if (collisionLayer) {
		if (!a_projectile->CollidesWithSmallTransparentLayer()) {
			excludedLayers = 0x15C15160;
		}
	} else {
		for (auto* layer : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::BGSCollisionLayer>()) {
			if (layer && layer->collisionIdx == 6) {
				collisionLayer = layer;
				break;
			}
		}
	}

	a_pick.customCollideLayers = 0x40000000;
	if (collisionLayer) {
		for (auto* layer : collisionLayer->collidesWith) {
			if (layer && layer->collisionIdx < 64) {
				a_pick.customCollideLayers |= 1ULL << layer->collisionIdx;
			}
		}
	}
	a_pick.customCollideLayers &= ~excludedLayers;
	std::uint32_t collisionGroup = 6;
	if (a_excludeActor && a_actor->loadedData) {
		collisionGroup = a_actor->GetCurrentCollisionGroup();
	}
	a_pick.castQuery.m_filterData.m_collisionFilterInfo = (collisionGroup << 16) | 0x29;
}

class ScopedAllHitsCollector final
{
public:
	ScopedAllHitsCollector() noexcept
	{
		auto* collector = Get();
		REX::EMPLACE_VTABLE<RE::hknpAllHitsCollector>(collector);
		collector->m_hits.m_data = collector->m_hits.m_storage;
		collector->m_hits.m_size = 0;
		collector->m_hits.m_capacityAndFlags = 0x8000000A;

		using reset_t = void(RE::hknpAllHitsCollector*);
		static REL::Relocation<reset_t> reset{ REL::ID{ 1360564, 2189457 } };
		reset(collector);
	}

	~ScopedAllHitsCollector() noexcept
	{
		// Flag 0 destroys the Havok object without deleting this inline storage.
		// Havok still frees m_hits.m_data if AddHit grew it past the 10 inline hits.
		using scalar_dtor_t = void*(RE::hknpAllHitsCollector*, std::uint32_t);
		static REL::Relocation<scalar_dtor_t> scalarDtor{ REL::ID{ 1555028, 2189439 } };
		(void)scalarDtor(Get(), 0);
	}

	ScopedAllHitsCollector(const ScopedAllHitsCollector&) = delete;
	ScopedAllHitsCollector(ScopedAllHitsCollector&&) = delete;
	ScopedAllHitsCollector& operator=(const ScopedAllHitsCollector&) = delete;
	ScopedAllHitsCollector& operator=(ScopedAllHitsCollector&&) = delete;

	[[nodiscard]] RE::hknpAllHitsCollector* Get() noexcept
	{
		return reinterpret_cast<RE::hknpAllHitsCollector*>(storage.data());
	}

private:
	alignas(RE::hknpAllHitsCollector) std::array<std::byte, sizeof(RE::hknpAllHitsCollector)> storage{};
};

inline bool GetPickData(const RE::NiPoint3& a_start, const RE::NiPoint3& a_end, RE::Actor* a_actor,
	RE::BGSProjectile* a_projectile, RE::bhkPickData& a_pick, RE::hknpAllHitsCollector& a_collector,
	bool a_excludeActor = true)
{
	if (!a_actor || !a_actor->parentCell || !a_actor->parentCell->GetbhkWorld()) {
		return false;
	}
	a_pick.SetStartEnd(a_start, a_end);
	a_pick.collector = std::addressof(a_collector);
	a_pick.collectorType = static_cast<RE::bhkPickData::COLLECTOR_TYPE>(3);
	SetupFilter(a_pick, a_actor, a_projectile, a_excludeActor);
	return RE::CombatUtilities::CalculateProjectileLOS(a_actor, a_projectile, a_pick);
}

inline RE::BSTEventSource<RE::TESEquipEvent>* GetEquipEventSource()
{
	static REL::Relocation<std::uintptr_t> source{ REL::ID{ 485633, 4798533 } };
	return reinterpret_cast<RE::BSTEventSource<RE::TESEquipEvent>*>(source.address());
}

inline RE::BGSMaterialType* GetMaterialTypeByName(const std::string& a_materialName)
{
	const RE::BSFixedString name{ a_materialName };
	for (auto* form : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::BGSMaterialType>()) {
		if (form && form->materialName == name) {
			return form;
		}
	}
	return nullptr;
}

inline RE::TESForm* GetFormFromMod(const std::string& a_modName, std::uint32_t a_formID)
{
	if (a_modName.empty() || a_formID == 0) {
		return nullptr;
	}
	return RE::TESDataHandler::GetSingleton()->LookupForm(a_formID, a_modName);
}
