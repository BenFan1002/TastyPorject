
//void format_output(const char *input) {
//    int indent = 0;
//    int in_string = 0;
//
//    while (*input) {
//        switch (*input) {
//            case '{':
//            case '[':
//                if (!in_string) {
//                    putchar(*input);
//                    putchar('\n');
//                    indent += 2;
//                    for (int i = 0; i < indent; i++) putchar(' ');
//                } else {
//                    putchar(*input);
//                }
//                break;
//
//            case '}':
//            case ']':
//                if (!in_string) {
//                    putchar('\n');
//                    indent -= 2;
//                    for (int i = 0; i < indent; i++) putchar(' ');
//                    putchar(*input);
//                } else {
//                    putchar(*input);
//                }
//                break;
//
//            case ',':
//                if (!in_string) {
//                    putchar(*input);
//                    putchar('\n');
//                    for (int i = 0; i < indent; i++) putchar(' ');
//                } else {
//                    putchar(*input);
//                }
//                break;
//
//            case '"':
//                in_string = !in_string;
//                putchar(*input);
//                break;
//
//            default:
//                putchar(*input);
//                break;
//        }
//        input++;
//    }
//
//}
