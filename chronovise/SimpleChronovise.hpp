#ifndef SIMPLE_CHRONOVISE_HPP_
#define SIMPLE_CHRONOVISE_HPP_

#include "chronovise/sec.hpp"


class SimpleChronovise : public chronovise::SimpleExecutionContext<unsigned int, double> {

public:
    SimpleChronovise(std::vector<unsigned long> & input) noexcept;

    virtual ~SimpleChronovise() = default;

    virtual exit_code_t onSetup() noexcept override;
    virtual exit_code_t onConfigure() noexcept override;
    virtual exit_code_t onRun() noexcept override;
    virtual exit_code_t onMonitor() noexcept override;
    virtual exit_code_t onRelease() noexcept override;

    private:
    std::vector<unsigned long> & data;
    std::vector<unsigned long>::iterator it;

};

#endif
