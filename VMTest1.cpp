#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <ctime>
#include <cstdlib> 
#include <string>
#include <limits>
#include <cstring>
#include <cerrno> 
#include <algorithm> 
#include <cctype>

using namespace std;

struct Item {
    int id;
    string name;
    int price;
    int stock;
    int discount;
};

const int FLASH_SALE_START = 12;
const int FLASH_SALE_END = 13;
const int FLASH_DISCOUNT = 30;
vector<int> flashSaleProductIDs = { 2, 4 };

const string ADMIN_PASSWORD = "admin123";

bool fileExists(const string& filename) {
    ifstream f(filename.c_str());
    return f.good();
}

void createDefaultCSV(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Gagal membuat file default: " << filename << " (errno=" << errno << ")\n";
        return;
    }
    file << "id,nama,harga,stok,diskon\n";
    file << "1,Air Mineral,5000,10,0\n";
    file << "2,Teh Botol,7000,5,0\n";
    file << "3,Kopi Susu,8000,3,0\n";
    file << "4,Coklat Bar,6000,4,0\n";
    file.close();
    cout << "[INFO] File \"" << filename << "\" dibuat otomatis.\n";
}

vector<Item> loadItemsFromCSV(const string& filename) {
    vector<Item> items;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Gagal membuka file: " << filename << endl;
        return items;
    }

    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idStr, name, priceStr, stockStr, discountStr;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, priceStr, ',');
        getline(ss, stockStr, ',');
        getline(ss, discountStr, ',');

        if (idStr.empty()) continue;

        Item item{};
        try {
            item.id = stoi(idStr);
            item.name = name;
            item.price = stoi(priceStr);
            item.stock = stoi(stockStr);
            item.discount = stoi(discountStr);
        }
        catch (...) {
            continue;
        }
        items.push_back(item);
    }
    file.close();
    return items;
}

void saveItemsToCSV(const string& filename, const vector<Item>& items) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Gagal menyimpan file: " << filename << endl;
        return;
    }

    file << "id,nama,harga,stok,diskon\n";
    for (const auto& item : items) {
        file << item.id << "," << item.name << "," << item.price << ","
            << item.stock << "," << item.discount << "\n";
    }
    file.close();
}

bool isFlashSaleTime() {
    time_t now = time(nullptr);
    tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    int hour = localTime.tm_hour;
    return (hour >= FLASH_SALE_START && hour < FLASH_SALE_END);
}

void applyFlashSale(vector<Item>& items) {
    for (auto& item : items)
        item.discount = 0;

    if (isFlashSaleTime()) {
        for (auto& item : items) {
            for (int id : flashSaleProductIDs)
                if (item.id == id)
                    item.discount = FLASH_DISCOUNT;
        }
        cout << "\n[FLASH SALE] Diskon "
            << FLASH_DISCOUNT << "% untuk produk tertentu!\n";
    }
}

void showItems(const vector<Item>& items) {
    cout << "\n=== DAFTAR PRODUK ===\n";
    cout << left << setw(5) << "ID"
        << left << setw(25) << "Nama"
        << right << setw(12) << "Harga"
        << right << setw(10) << "Diskon"
        << right << setw(7) << "Stok" << endl;
    cout << "-------------------------------------------------------------\n";
    for (const auto& item : items) {
        int hargaDiskon = item.price * (100 - item.discount) / 100;

        string hargaStr = "Rp" + to_string(hargaDiskon);
        string diskonStr = (item.discount > 0 ? to_string(item.discount) + "%" : "-");

        cout << left << setw(5) << item.id
            << left << setw(25) << item.name
            << right << setw(12) << hargaStr
            << right << setw(10) << diskonStr
            << right << setw(7) << item.stock << endl;
    }
}

Item* findItemById(vector<Item>& items, int id) {
    for (auto& item : items)
        if (item.id == id)
            return &item;
    return nullptr;
}

bool adminLogin() {
    string password;
    cout << "\n=== ADMIN LOGIN ===\n";
    cout << "Masukkan Password: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, password);

    if (password == ADMIN_PASSWORD) {
        cout << "Login Admin Berhasil! Selamat datang.\n";
        return true;
    }
    else {
        cout << "Password Salah! Akses ditolak.\n";
        return false;
    }
}

void prosesDonasi(int& kembali) {
    const string DONASI_FILENAME = "donasi.txt";
    const string NAMA_YAYASAN = "Yayasan Harapan Masa Depan";

    cout << "\n==============================================\n";
    cout << "DONASI KEMBALIAN AKTIF - Terdapat Rp" << kembali << " kembalian.\n";
    cout << "Apakah Anda bersedia mendonasikan Rp" << kembali
        << " ini kepada " << NAMA_YAYASAN << "?\n";

    cout << "(Y) Donasikan\n";
    cout << "(N) Terima Uang Kembalian\n";
    cout << "Pilih (Y/N): ";

    char pilihChar;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (!(cin >> pilihChar)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\nInput tidak valid. Kembalian Rp" << kembali << " diberikan kembali.\n";
        return;
    }

    pilihChar = tolower(pilihChar);

    if (pilihChar == 'y') {
        ofstream file(DONASI_FILENAME, ios::app);
        if (file.is_open()) {
            time_t now = time(nullptr);
            tm localTime{};
#ifdef _WIN32
            localtime_s(&localTime, &now);
#else
            localtime_r(&now, &localTime);
#endif
            file << put_time(&localTime, "%d-%m-%Y %H:%M:%S") << " - Donasi Rp" << kembali
                << " ke " << NAMA_YAYASAN << "\n";
            file.close();
            cout << "\nTerima kasih! Rp" << kembali
                << " telah didonasikan ke " << NAMA_YAYASAN << ".\n";
            kembali = 0;
        }
        else {
            cerr << "Gagal mencatat donasi ke file. Kembalian diberikan kembali.\n";
        }
    }
    else if (pilihChar == 'n') {
        cout << "\nAnda memilih untuk menerima uang kembalian sebesar Rp " << kembali << ".\n";
    }
    else {
        cout << "\nInput tidak dikenali. Kembalian Rp" << kembali << " diberikan kembali.\n";
    }
    cout << "==============================================\n";
}

void simpanStrukKeFile(const vector<pair<Item*, int>>& keranjang,
    int total, int uang, int kembalian, int diskonPembayaran) {
    ofstream file("struk.txt", ios::app);
    if (!file.is_open()) {
        cerr << "Gagal membuka file struk.txt\n";
        return;
    }

    time_t now = time(nullptr);
    tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    file << "\n==============================\n";
    file << "STRUK PEMBELIAN (" << put_time(&localTime, "%d-%m-%Y %H:%M:%S") << ")\n";
    file << "==============================\n";
    for (auto& p : keranjang) {
        Item* item = p.first;
        int qty = p.second;
        int hargaDiskon = item->price * (100 - item->discount) / 100;
        int subtotal = hargaDiskon * qty;
        file << item->name << " x" << qty << " = Rp" << subtotal << "\n";
    }
    if (diskonPembayaran > 0)
        file << "Diskon tambahan pembayaran: -" << diskonPembayaran << "%\n";

    file << "TOTAL : Rp" << total << "\n";
    file << "DIBAYAR: Rp" << uang << "\n";
    file << "KEMBALIAN: Rp" << kembalian << "\n";
    file << "==============================\n\n";
    file.close();
}

void mysteryProduct(vector<Item>& items, const string& filename) {
    if (items.empty()) {
        cout << "Tidak ada produk tersedia untuk Mystery Product.\n";
        return;
    }
    srand((unsigned)time(nullptr));
    int randomIndex = rand() % items.size();
    Item& mystery = items[randomIndex];
    int discount = 10 + rand() % 41;

    int originalDiscount = mystery.discount;
    mystery.discount = discount;
    saveItemsToCSV(filename, items);

    cout << "\n[MYSTERY] Produk misterius hari ini: **" << mystery.name << "**\n";
    cout << "Diskon spesial **" << discount << "%** telah diterapkan dan disimpan ke file!\n";
}

void tekaTeki(vector<Item>& items, const string& filename) {
    if (items.empty()) {
        cout << "Tidak ada produk tersedia.\n";
        return;
    }
    srand((unsigned)time(nullptr));
    int i = rand() % items.size();
    Item& t = items[i];

    string teka;
    if (t.name.find("Air") != string::npos)
        teka = "Aku cair dan menyegarkan, tapi bukan teh. Siapakah aku?";
    else if (t.name.find("Teh") != string::npos)
        teka = "Aku manis dan sering disajikan dalam botol. Siapakah aku?";
    else if (t.name.find("Kopi") != string::npos)
        teka = "Aku mengandung kafein dan susu. Siapakah aku?";
    else if (t.name.find("Coklat") != string::npos)
        teka = "Aku manis, padat, dan bisa membuatmu bahagia. Siapakah aku?";
    else
        teka = "Aku produk rahasia dari mesin ini. Siapakah aku?";

    cout << "\nTeka-teki: " << teka << "\nJawaban: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string jawaban;
    getline(cin, jawaban);

    string correct = t.name;
    transform(jawaban.begin(), jawaban.end(), jawaban.begin(), ::tolower);
    transform(correct.begin(), correct.end(), correct.begin(), ::tolower);

    if (jawaban.find(correct.substr(0, correct.find(' '))) != string::npos || jawaban.find(correct) != string::npos) {
        cout << "Benar! Anda mendapatkan produk gratis!\n";

        int randomIndex = rand() % items.size();
        Item& hadiah = items[randomIndex];

        if (hadiah.stock > 0) {
            hadiah.stock--;
            cout << "Anda mendapatkan 1 " << hadiah.name << " gratis!\n";
            saveItemsToCSV(filename, items);
        }
        else {
            cout << "Produk hadiah (" << hadiah.name << ") sedang habis stok. Coba lagi nanti.\n";
        }
    }
    else {
        cout << "Salah, jawabannya adalah: " << t.name << endl;
    }
}

void tambahProduk(vector<Item>& items, const string& filename) {
    if (!adminLogin()) return;

    Item n{};
    n.id = items.empty() ? 1 : items.back().id + 1;
    cout << "Nama produk: ";
    getline(cin, n.name);
    cout << "Harga: Rp";
    while (!(cin >> n.price) || n.price <= 0) {
        cout << "Harga tidak valid. Masukkan harga (angka > 0): Rp";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << "Stok: ";
    while (!(cin >> n.stock) || n.stock < 0) {
        cout << "Stok tidak valid. Masukkan stok (angka >= 0): ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    n.discount = 0;
    items.push_back(n);
    saveItemsToCSV(filename, items);
    cout << "Produk baru berhasil ditambahkan! ID: " << n.id << endl;
}

void rollbackStok(vector<pair<Item*, int>>& keranjang) {
    for (auto& k : keranjang) {
        Item* item = k.first;
        int qty = k.second;
        item->stock += qty;
    }
}

void processPurchase(vector<Item>& items, const string& filename) {
    vector<pair<Item*, int>> keranjang;
    char lagi = 'y';
    bool validSelection = false;

    while ((lagi == 'y' || lagi == 'Y') && !items.empty()) {
        int id;
        cout << "\nMasukkan ID produk yang ingin dibeli: ";
        if (!(cin >> id)) {
            cout << "Input ID tidak valid. Batalkan pemilihan produk.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }

        Item* p = findItemById(items, id);
        if (!p) {
            cout << "Produk tidak ditemukan!\n";
            cout << "Lanjut tambah produk lain? (y/n): ";
            cin >> lagi;
            continue;
        }
        if (p->stock <= 0) {
            cout << "Stok " << p->name << " habis!\n";
            cout << "Lanjut tambah produk lain? (y/n): ";
            cin >> lagi;
            continue;
        }

        int qty;
        cout << "Masukkan jumlah (" << p->name << ") (Stok " << p->stock << "): ";
        if (!(cin >> qty)) {
            cout << "Input jumlah tidak valid. Batalkan pemilihan produk.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }

        if (qty <= 0 || qty > p->stock) {
            cout << "Jumlah tidak valid atau stok kurang!\n";
            cout << "Lanjut tambah produk lain? (y/n): ";
            cin >> lagi;
            continue;
        }

        keranjang.push_back({ p, qty });
        p->stock -= qty;

        validSelection = true;
        cout << "Ditambahkan ke keranjang: " << qty << " x " << p->name << endl;
        cout << "Stok tersisa untuk " << p->name << ": " << p->stock << endl;

        cout << "\nTambah produk lain? (y/n): ";
        cin >> lagi;
    }

    if (!validSelection || keranjang.empty()) {
        cout << "\nKeranjang kosong. Pembelian dibatalkan.\n";
        rollbackStok(keranjang);
        saveItemsToCSV(filename, items);
        return;
    }

    cout << "\n" << string(64, '=') << endl;
    cout << "Keranjang sudah terisi.\n";
    cout << "Lanjut ke Pembayaran (Y) atau Batalkan semua (N)? (Y/N): ";

    char finalChoice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (!(cin >> finalChoice)) {
        finalChoice = 'n';
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    finalChoice = tolower(finalChoice);

    if (finalChoice == 'n') {
        cout << "\nTransaksi Dibatalkan. Semua produk dikembalikan ke stok.\n";
        rollbackStok(keranjang);
        saveItemsToCSV(filename, items);
        cout << string(64, '=') << endl;
        return;
    }

    int total_sebelum_diskon_global = 0;
    int total_setelah_diskon_global = 0;

    const string LINE_SEPARATOR = "----------------------------------------------------------------";
    const string BORDER_SEPARATOR = "================================================================";

    cout << "\n\n" << BORDER_SEPARATOR << endl;
    cout << "                          RINGKASAN PEMBELIAN\n";
    cout << BORDER_SEPARATOR << endl;

    cout << left << setw(20) << "Nama Produk"
        << right << setw(5) << "Qty"
        << right << setw(15) << "Harga Awal"
        << right << setw(10) << "Diskon"
        << right << setw(14) << "Subtotal\n";
    cout << LINE_SEPARATOR << endl;

    for (auto& k : keranjang) {
        Item* item = k.first;
        int qty = k.second;

        int harga_awal = item->price;
        int diskon_persen = item->discount;
        int harga_setelah_diskon = harga_awal * (100 - diskon_persen) / 100;
        int subtotal_setelah_diskon = harga_setelah_diskon * qty;

        total_sebelum_diskon_global += subtotal_setelah_diskon;

        string hargaAwalStr = "Rp" + to_string(harga_awal);
        string diskonStr = (diskon_persen > 0 ? to_string(diskon_persen) + "%" : "-");
        string subtotalStr = "Rp" + to_string(subtotal_setelah_diskon);

        cout << left << setw(20) << item->name
            << right << setw(5) << qty
            << right << setw(15) << hargaAwalStr
            << right << setw(10) << diskonStr
            << right << setw(14) << subtotalStr << endl;
    }

    cout << LINE_SEPARATOR << endl;

    string subtotalFinalStr = "Rp" + to_string(total_sebelum_diskon_global);
    cout << left << setw(48) << "SUBTOTAL (Setelah Diskon Produk)"
        << right << setw(16) << subtotalFinalStr << endl;

    int diskonPembayaran = 0;
    if (total_sebelum_diskon_global > 20000) {
        diskonPembayaran = 10;
        int jumlah_diskon_tambahan = total_sebelum_diskon_global * diskonPembayaran / 100;
        total_setelah_diskon_global = total_sebelum_diskon_global - jumlah_diskon_tambahan;

        string diskonLabel = "Diskon Pembayaran Global (" + to_string(diskonPembayaran) + "%)";
        string diskonValueStr = "-Rp" + to_string(jumlah_diskon_tambahan);

        cout << left << setw(48) << diskonLabel
            << right << setw(16) << diskonValueStr << endl;
    }
    else {
        total_setelah_diskon_global = total_sebelum_diskon_global;
    }

    cout << LINE_SEPARATOR << endl;

    string totalFinalStr = "Rp" + to_string(total_setelah_diskon_global);
    cout << left << setw(48) << "**TOTAL AKHIR YANG HARUS DIBAYAR**"
        << right << setw(16) << totalFinalStr << endl;
    cout << BORDER_SEPARATOR << endl;

    int total = total_setelah_diskon_global;
    int uang;

    cout << "\nMasukkan uang Anda: Rp";
    if (!(cin >> uang)) {
        cout << "\nInput uang tidak valid. Transaksi dibatalkan. Stok dikembalikan.\n";
        rollbackStok(keranjang);
        saveItemsToCSV(filename, items);
        return;
    }

    if (uang < total) {
        cout << "Uang tidak cukup. Transaksi dibatalkan. Stok dikembalikan.\n";
        rollbackStok(keranjang);
        saveItemsToCSV(filename, items);
        return;
    }

    saveItemsToCSV(filename, items);

    int kembali = uang - total;

    if (kembali > 0 && kembali < 1000) {
        prosesDonasi(kembali);
    }

    if (kembali > 0) {
        cout << "\nTransaksi berhasil! Kembalian: Rp" << kembali << endl;
    }
    else {
        cout << "\nTransaksi berhasil! Kembalian: Rp0 (Uang pas atau didonasikan)\n";
    }

    simpanStrukKeFile(keranjang, total, uang, kembali, diskonPembayaran);
    cout << "Struk pembelian telah disimpan ke file **struk.txt**.\n";
}

int main() {
    string filename = "produk.csv";
    if (!fileExists(filename))
        createDefaultCSV(filename);

    vector<Item> items = loadItemsFromCSV(filename);
    applyFlashSale(items);

    int pilih;
    do {
        applyFlashSale(items);

        cout << "\n==== MENU VENDING MACHINE ====\n";
        cout << "1. Beli Produk (Lihat & Beli)\n";
        cout << "2. Tambah produk baru (Admin)\n";
        cout << "3. Mystery Product\n";
        cout << "4. Teka-Teki Berhadiah\n";
        cout << "0. Keluar\n";
        cout << "Pilih menu: ";

        if (!(cin >> pilih)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Input tidak valid. Mohon masukkan angka saja.\n";
            pilih = -1;
            continue;
        }

        switch (pilih) {
        case 1:
        {
            char lanjutLagi = 'y';

            while (lanjutLagi == 'y' || lanjutLagi == 'Y') {
                cout << "\n================================\n";
                showItems(items);

                char lanjutBeli;
                cout << "\nLanjut ke proses pemilihan/pembelian? (y/n): ";

                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (!(cin >> lanjutBeli)) {
                    lanjutBeli = 'n';
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                lanjutBeli = tolower(lanjutBeli);

                if (lanjutBeli == 'y') {
                    processPurchase(items, filename);

                    cout << "\n================================\n";
                    cout << "Mau lanjut sesi pembelian lagi? (y/n): ";

                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (!(cin >> lanjutLagi)) {
                        lanjutLagi = 'n';
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                    lanjutLagi = tolower(lanjutLagi);

                }
                else {
                    cout << "\nPembelian dibatalkan. Kembali ke Menu Utama.\n";
                    lanjutLagi = 'n';
                }
            }
        }
        break;
        case 2: tambahProduk(items, filename); break;
        case 3: mysteryProduct(items, filename); break;
        case 4: tekaTeki(items, filename); break;
        case 0: cout << "Terima kasih telah menggunakan mesin ini! Bye~\n"; break;
        default: cout << "Pilihan tidak valid, coba yang lain ya.\n"; break;
        }
    } while (pilih != 0);

    return 0;

    // Egi - Dian jawab woy
}