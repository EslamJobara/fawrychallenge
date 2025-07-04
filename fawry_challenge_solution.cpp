#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <cmath>

using namespace std;

// ========== واجهة الشحن ==========
class Shippable {
public:
    virtual string getName() const = 0;
    virtual double getWeight() const = 0;
    virtual ~Shippable() {}
};

// ========== تعريف المنتج ==========
class Product {
protected:
    string name;
    double price;
    int quantity;

public:
    Product(string n, double p, int q) : name(n), price(p), quantity(q) {}

    virtual bool isAvailable(int requested) const {
        return quantity >= requested;
    }

    virtual bool isExpired() const {
        return false;
    }

    virtual bool requiresShipping() const {
        return true;
    }

    virtual double getWeight() const {
        return 0.0;
    }

    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void reduceQuantity(int q) { quantity -= q; }

    virtual ~Product() {}
};

// ========== منتج رقمي ==========
class DigitalProduct : public Product {
public:
    DigitalProduct(string n, double p, int q) : Product(n, p, q) {}

    bool requiresShipping() const override {
        return false;
    }
};

// ========== منتج فعلي ==========
class PhysicalProduct : public Product, public Shippable {
    double weight;
    time_t expiration;

public:
    PhysicalProduct(string n, double p, int q, double w, time_t exp = 0)
        : Product(n, p, q), weight(w), expiration(exp) {}

    double getWeight() const override {
        return weight;
    }

    bool isExpired() const override {
        if (expiration == 0) return false;
        return time(nullptr) > expiration;
    }

    string getName() const override {
        return name;
    }
};

// ========== العنصر في السلة ==========
struct CartItem {
    Product* product;
    int quantity;

    double totalPrice() const {
        return product->getPrice() * quantity;
    }
};

// ========== السلة نفسها ==========
class ShoppingCart {
    vector<CartItem> items;

public:
    void addItem(Product* product, int quantity) {
        if (quantity <= 0)
            throw runtime_error("غير متاح");

        if (!product->isAvailable(quantity))
            throw runtime_error("الكمية المطلوبة غير متوفرة.");

        if (product->isExpired())
            throw runtime_error("المنتج منتهي الصلاحية.");

        for (auto& item : items) {
            if (item.product == product) {
                item.quantity += quantity;
                return;
            }
        }

        items.push_back({product, quantity});
    }

    bool isEmpty() const {
        return items.empty();
    }

    double getSubtotal() const {
        double total = 0;
        for (const auto& item : items)
            total += item.totalPrice();
        return total;
    }

    vector<Shippable*> getShippableItems() const {
        vector<Shippable*> result;
        for (const auto& item : items) {
            if (item.product->requiresShipping()) {
                auto* shippable = dynamic_cast<Shippable*>(item.product);
                for (int i = 0; i < item.quantity; ++i)
                    result.push_back(shippable);
            }
        }
        return result;
    }

    const vector<CartItem>& getItems() const {
        return items;
    }

    void clear() {
        items.clear();
    }
};

// ========== العميل ==========
class Customer {
    string name;
    double balance;

public:
    Customer(string n, double b) : name(n), balance(b) {}

    void deduct(double amount) {
        if (balance < amount)
            throw runtime_error("الرصيد غير كاف.");
        balance -= amount;
    }

    double getBalance() const { return balance; }
    string getName() const { return name; }
};

// ========== الشحن ==========
class ShippingService {
public:
    static double calculateFee(const vector<Shippable*>& items) {
        double totalWeight = 0;
        for (auto item : items)
            totalWeight += item->getWeight();
        return ceil(totalWeight * 30);
    }

    static void ship(const vector<Shippable*>& items) {
        cout << "\n** إشعار الشحن **\n";
        map<string, double> itemWeights;
        for (auto item : items)
            itemWeights[item->getName()] += item->getWeight();

        double totalWeight = 0;
        for (const auto& [name, weight] : itemWeights) {
            cout << name << setw(20 - name.size()) << fixed << setprecision(1)
                 << weight * 1000 << " جم\n";
            totalWeight += weight;
        }

        cout << "إجمالي الوزن: " << fixed << setprecision(1) << totalWeight << " كجم\n\n";
    }
};

// ========== عملية الشراء ==========
void checkout(Customer& customer, ShoppingCart& cart) {
    if (cart.isEmpty()) {
        cout << "السلة فارغة.\n\n";
        return;
    }

    try {
        double subtotal = cart.getSubtotal();
        auto shippables = cart.getShippableItems();
        double shipping = ShippingService::calculateFee(shippables);
        double total = subtotal + shipping;

        customer.deduct(total);

        if (!shippables.empty())
            ShippingService::ship(shippables);

        cout << "** إيصال الدفع **\n";
        for (const auto& item : cart.getItems()) {
            cout << item.quantity << "x " << item.product->getName()
                 << setw(15 - item.product->getName().size())
                 << fixed << setprecision(2) << item.totalPrice() << " جنيه\n";
            item.product->reduceQuantity(item.quantity);
        }

        cout << "------------------------\n";
        cout << "المجموع:      " << subtotal << "\n";
        cout << "الشحن:        " << shipping << "\n";
        cout << "الإجمالي:     " << total << "\n";
        cout << "الرصيد المتبقي: " << customer.getBalance() << "\n\n";

        cart.clear();
    } catch (const exception& e) {
        cout << "خطأ أثناء الدفع: " << e.what() << "\n\n";
    }
}

// ========== MAIN ==========
int main() {
    time_t now = time(nullptr);
    time_t validDate = now + 86400 * 30;
    time_t expiredDate = now - 86400 * 10;

    PhysicalProduct cheese("جبنة", 100, 10, 0.2, validDate);
    PhysicalProduct biscuits("بسكويت", 150, 5, 0.7, validDate);
    PhysicalProduct tv("تلفزيون", 500, 3, 3.5);
    DigitalProduct card("كارت شحن", 25, 20);
    PhysicalProduct expiredCheese("جبنة قديمة", 80, 2, 0.2, expiredDate);

    Customer customer("أحمد", 1000);
    ShoppingCart cart;

    cart.addItem(&cheese, 2);
    cart.addItem(&biscuits, 1);
    cart.addItem(&card, 1);
    checkout(customer, cart);

    return 0;
}
