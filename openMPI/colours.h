namespace colors {
    namespace {
        std::string list [] =  {"blue", "red", "green", "yellow", "lightblue", "purple", "orange", "violet", "aqua", "darksalmon",
                                "darkcyan", "dodgerblue", "lime", "fuchsia", "darkgoldenrod", "saddlebrown", "orchid", "lemonchiffon",
                                "cadetblue", "burlywood", "plum", "tomato", "teal", "darkolivegreen", "gray", "black", "floralwhite"};
    }
    std::string get_colour (unsigned index) {
        if (index >= list->length()){
            return "";
        }
        return list[index];
    }

}

