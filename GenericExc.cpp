#include "GenericExc.h"

const u::uint32 GenericExc::ERR_NOT_SET = (u::uint32)-1;

GenericExc::GenericExc(const QString& what, u::uint32 err)
    : what_(what)
    , err_(err) {
}

GenericExc::GenericExc(const GenericExc& r)
    : what_(r.what_)
    , err_(r.err_) {
}

GenericExc::~GenericExc() {
}

GenericExc& GenericExc::operator=(const GenericExc& r) {
    if (this != &r) {
        what_ = r.what_;
        err_ = r.err_;
    }
    return *this;
}
