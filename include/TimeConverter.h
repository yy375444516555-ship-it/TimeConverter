#pragma once

#include "DataContainers.h"
#include "Time.h"

template<typename DutContainer, typename DatContainer>
class TimeConverter {
private:
    DutContainer dutContainer_;
    DatContainer datContainer_;
    SomeTcbContainer tcbContainer_;

public:
    TimeConverter(
        const DutContainer& dutContainer,
        const DatContainer& datContainer,
        const SomeTcbContainer& tcbContainer
    );

    template<Scale To, Scale From>
    Time<To> convert(const Time<From>& from) const;
};

using DefaultTimeConverter = TimeConverter<SomeDutContainer, SomeDATContainer>;
extern template class TimeConverter<SomeDutContainer, SomeDATContainer>;
