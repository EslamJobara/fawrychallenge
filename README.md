# Fawry Quantum Internship Challenge - E-Commerce System

## 👨‍💻 Author
**Eslam Jobara**

## 📌 Project Overview
This project is a simplified e-commerce system developed in C++. It is part of the Fawry Quantum Full Stack Internship Challenge.  
The goal was to simulate a shopping process that handles both digital and physical products, with rules related to product availability, shipping, and checkout validation.

## ✅ Features Implemented

- **Product Management**
  - Products have `name`, `price`, `quantity`.
  - Some products expire (like Cheese), others don’t (like TV).
  - Some products require shipping and provide weight info.

- **Cart Functionality**
  - Add items to cart while checking for:
    - Available quantity.
    - Expiry status.
  - Calculates subtotal and shipping.

- **Checkout Logic**
  - Validates if:
    - Cart is not empty.
    - Customer has enough balance.
    - Products are valid (not expired/out of stock).
  - Calculates:
    - Subtotal.
    - Shipping (30 EGP/kg, rounded up).
    - Total.
  - Deducts from customer balance.
  - Prints receipt and shipping notice.

- **ShippingService**
  - Only accepts items that implement a `Shippable` interface (i.e. physical products).
  - Outputs total shipment weight.

## 🧪 Test Scenario (Sample Output)
```plaintext
