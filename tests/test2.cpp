#include <string>
#include <functional>

const std::function<std::string(const char*)> G_TRANSLATION_FUN;

int main(int argc, char** argv)
{
   const char* input = "Hello, world!";
   std::string output = G_TRANSLATION_FUN(input);
   if (output != input)
      exit(1);
   exit(0);
}