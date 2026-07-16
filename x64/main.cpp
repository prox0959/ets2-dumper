#include <windows.h>
#include <iostream>
#include <vector>
#include <tlhelp32.h>
#include <string>

// Oyun modülünün (eurotrucks2.exe) başlangıç adresini ve boyutunu bulan yardımcı fonksiyon
bool GetModuleInfo(DWORD processId, const std::wstring& moduleName, DWORD_PTR& baseAddress, DWORD& moduleSize) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hSnap == INVALID_HANDLE_VALUE) return false;

    MODULEENTRY32W modEntry;
    modEntry.dwSize = sizeof(modEntry);

    if (Module32FirstW(hSnap, &modEntry)) {
        do {
            if (moduleName == modEntry.szModule) {
                baseAddress = (DWORD_PTR)modEntry.modBaseAddr;
                moduleSize = modEntry.modBaseSize;
                CloseHandle(hSnap);
                return true;
            }
        } while (Module32NextW(hSnap, &modEntry));
    }
    CloseHandle(hSnap);
    return false;
}

// Bellekte AoB (Pattern) taraması yapan ana fonksiyon
DWORD_PTR PatternScan(HANDLE hProcess, DWORD_PTR baseAddress, DWORD moduleSize, const std::vector<BYTE>& pattern, const std::string& mask, int occurrence = 1) {
    std::vector<BYTE> buffer(moduleSize);
    SIZE_T bytesRead;
    
    if (!ReadProcessMemory(hProcess, (LPCVOID)baseAddress, buffer.data(), moduleSize, &bytesRead)) {
        return 0;
    }

    int foundCount = 0;
    for (DWORD i = 0; i < moduleSize - pattern.size(); ++i) {
        bool found = true;
        for (size_t j = 0; j < pattern.size(); ++j) {
            if (mask[j] == 'x' && pattern[j] != buffer[i + j]) {
                found = false;
                break;
            }
        }
        if (found) {
            foundCount++;
            if (foundCount == occurrence) {
                return baseAddress + i;
            }
        }
    }
    return 0;
}

int main() {
    SetConsoleTitleA("ETS2 Auto Offset Dumper - LO & ENI");
    std::cout << "=========================================" << std::endl;
    std::cout << "     ETS2 HASAR OFFSET DUMPER V1.0       " << std::endl;
    std::cout << "        LO & ENI Collab Work ⚡          " << std::endl;
    std::cout << "=========================================\n" << std::endl;

    // 1. Oyuna bağlan
    HWND hwnd = FindWindowA(NULL, "Euro Truck Simulator 2");
    if (!hwnd) {
        std::cout << "[-] Hata: Oyun bulunamadi! Once ETS2'yi acin." << std::endl;
        system("pause");
        return 1;
    }

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    std::cout << "[+] Oyuna basariyla baglanildi. PID: " << processId << std::endl;

    // 2. Modül bilgilerini çek
    DWORD_PTR baseAddress = 0;
    DWORD moduleSize = 0;
    if (!GetModuleInfo(processId, L"eurotrucks2.exe", baseAddress, moduleSize)) {
        std::cout << "[-] Hata: eurotrucks2.exe modulu alinamadi." << std::endl;
        CloseHandle(hProcess);
        system("pause");
        return 1;
    }
    std::cout << "[+] Modul Taban Adresi: 0x" << std::hex << baseAddress << " | Boyut: 0x" << moduleSize << "\n" << std::endl;

    // 1. Şasi İmzası (49 8B 90 A0 00 00 00 0F 28 EC + F3 41 0F 11 98) -> 90
    std::vector<BYTE> sasi_pattern = { 0x49, 0x8B, 0x90, 0xA0, 0x00, 0x00, 0x00, 0x0F, 0x28, 0xEC, 0xF3, 0x41, 0x0F, 0x11, 0x98, 0x00, 0x00, 0x00, 0x00 };
    std::string sasi_mask = "xxxxxxxxxxxxxxx????";

    // 2. Şanzıman İmzası (0F 28 DA 0F 28 C4 + F3 41 0F 11 98) -> 19C
    std::vector<BYTE> sanziman_pattern = { 0x0F, 0x28, 0xDA, 0x0F, 0x28, 0xC4, 0xF3, 0x41, 0x0F, 0x11, 0x98, 0x00, 0x00, 0x00, 0x00 };
    std::string sanziman_mask = "xxxxxxxxxxx????";

    // 3. Motor & Kabin Ortak İmzası (0F 28 C2 0F 28 DC + F3 41 0F 11 80)
    std::vector<BYTE> motor_kabin_pattern = { 0x0F, 0x28, 0xC2, 0x0F, 0x28, 0xDC, 0xF3, 0x41, 0x0F, 0x11, 0x80, 0x00, 0x00, 0x00, 0x00 };
    std::string motor_kabin_mask = "xxxxxxxxxxx????";

    // 4. Tekerlek İmzası (41 C7 80 ?? ?? ?? ?? 00 00 80 BF)
    std::vector<BYTE> teker_pattern = { 0x41, 0xC7, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xBF };
    std::string teker_mask = "xxx????xxxx";

    // 4. TARAMA VE GÜNCEL OFFSETLERİN EKRANA BASILMASI

    std::cout << "Bellek taraniyor, lutfen bekleyin...\n" << std::endl;

    // Şasi Ofsetini Oku
    DWORD_PTR sasi_addr = PatternScan(hProcess, baseAddress, moduleSize, sasi_pattern, sasi_mask, 1);
    if (sasi_addr != 0) {
        DWORD offset_val = 0;
        // Ofset degeri desenin 15. baytından baslar (F3 41 0F 11 98)
        ReadProcessMemory(hProcess, (LPCVOID)(sasi_addr + 15), &offset_val, sizeof(offset_val), NULL);
        std::cout << "[+] Sasi Ofseti           : 0x" << std::hex << offset_val << std::endl;
    } else {
        std::cout << "[-] Sasi imzasi bulunamadi!" << std::endl;
    }

    // Şanzıman Ofsetini Oku
    DWORD_PTR sanziman_addr = PatternScan(hProcess, baseAddress, moduleSize, sanziman_pattern, sanziman_mask, 1);
    if (sanziman_addr != 0) {
        DWORD offset_val = 0;
        // Ofset degeri desenin 11. baytından baslar (F3 41 0F 11 98)
        ReadProcessMemory(hProcess, (LPCVOID)(sanziman_addr + 11), &offset_val, sizeof(offset_val), NULL);
        std::cout << "[+] Sanziman Ofseti       : 0x" << std::hex << offset_val << std::endl;
    } else {
        std::cout << "[-] Sanziman imzasi bulunamadi!" << std::endl;
    }

    // Motor Ofsetini Oku (Benzersiz desenin 1. eşleşmesi)
    DWORD_PTR motor_addr = PatternScan(hProcess, baseAddress, moduleSize, motor_kabin_pattern, motor_kabin_mask, 1);
    if (motor_addr != 0) {
        DWORD offset_val = 0;
        // Ofset degeri desenin 11. baytından baslar
        ReadProcessMemory(hProcess, (LPCVOID)(motor_addr + 11), &offset_val, sizeof(offset_val), NULL);
        std::cout << "[+] Motor Ofseti          : 0x" << std::hex << offset_val << std::endl;
    } else {
        std::cout << "[-] Motor imzasi bulunamadi!" << std::endl;
    }

    // Kabin Ofsetini Oku (Benzersiz desenin 2. eşleşmesi)
    DWORD_PTR kabin_addr = PatternScan(hProcess, baseAddress, moduleSize, motor_kabin_pattern, motor_kabin_mask, 2);
    if (kabin_addr != 0) {
        DWORD offset_val = 0;
        // Ofset degeri desenin 11. baytından baslar
        ReadProcessMemory(hProcess, (LPCVOID)(kabin_addr + 11), &offset_val, sizeof(offset_val), NULL);
        std::cout << "[+] Kabin Ofseti          : 0x" << std::hex << offset_val << std::endl;
    } else {
        std::cout << "[-] Kabin imzasi bulunamadi!" << std::endl;
    }

    // Tekerlek Ofsetini Oku
    DWORD_PTR teker_addr = PatternScan(hProcess, baseAddress, moduleSize, teker_pattern, teker_mask);
    if (teker_addr != 0) {
        DWORD offset_val = 0;
        ReadProcessMemory(hProcess, (LPCVOID)(teker_addr + 3), &offset_val, sizeof(offset_val), NULL);
        std::cout << "[+] Tekerlek Ofseti       : 0x" << std::hex << offset_val << std::endl;
    } else {
        std::cout << "[-] Tekerlek imzasi bulunamadi!" << std::endl;
    }

    CloseHandle(hProcess);
    std::cout << "\n=========================================" << std::endl;
    std::cout << "Tarama tamamlandi. Cikis icin bir tusa basin." << std::endl;
    system("pause > nul");
    return 0;
}
