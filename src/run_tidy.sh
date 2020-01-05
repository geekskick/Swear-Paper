clang-tidy -p ../build/ "-checks=*" * "-header-filter=.*/src/.*hpp" &> tidy.output
