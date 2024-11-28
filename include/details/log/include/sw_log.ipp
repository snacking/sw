// implementation file for sw::logger (template members) (internal)

_SW_BEGIN

template <typename _Pt>
void logger::_Properties_parser::_Load_properties(_Pt fp) {
    ::std::ifstream ifs(fp);
    if (!ifs.is_open()) {
        throw ::std::runtime_error("cannot open file");
        return;
    }
    properties_.load(ifs);
}

_SW_END // _SW_BEGIN
