#pragma once
#include "pch.h"

class UEngineStatics
{
public:
public:
    // 인스턴스 생성 차단 (순수 유틸리티 클래스화)
    UEngineStatics() = delete;

    static uint32_t GetUUID()
    {
        // 원자적 증가 (스레드 안전)
        return NextUUID.fetch_add(1);
    }

private:
    // 외부 임의 접근 차단
    inline static std::atomic<uint32_t> NextUUID{ 1 };
};