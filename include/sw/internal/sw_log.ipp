// implementation file for sw::logger (template members) (internal)

_SW_BEGIN

template <typename _Pt>
void logger::_Properties_parser::_Load_properties(_Pt fp) _SW_THROW(::std::runtime_error) {
    ::std::ifstream ifs(fp);
    if (!ifs.is_open()) {
        _SW_THROW(::std::runtime_error("cannot open file"));
        return;
    }
    properties_.load(ifs);
}

_SW_END // _SW_BEGIN
