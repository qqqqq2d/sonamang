#include <iostream>
#include <cwctype> // for towupper
#include <locale>  // for wcout, wcin

int main() {
    std::wcout.imbue(std::locale("")); // Set locale to support local character encoding
    wchar_t ch = L'ö'; // Use wide character literal
    wchar_t upper_ch = std::towupper(ch); // Convert to uppercase
    std::wcout << L"Original: " << ch << L", Uppercase: " << upper_ch << std::endl;
    return 0;
}
