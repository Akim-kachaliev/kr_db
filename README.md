Курсовой проект по дисциплине: "Проектирование и администрирование баз данных"

# Информационная система для распределения дополнительных обязанностей

## 📌 О проекте

Данный проект представляет собой клиент-серверное приложение для автоматизации процесса распределения разовых дополнительных работ между сотрудниками компании. Система позволяет руководителям создавать задачи, назначать исполнителей, а сотрудникам — отмечать факт завершения работ. Вся логика расчёта доплат автоматизирована и завязана на квалификацию сотрудников и вид работ.

**Ключевые возможности:**
- Ролевая модель (Администратор, Руководитель, Сотрудник).
- Создание работ с привязкой к классификатору видов.
- Назначение одного или нескольких сотрудников на работу.
- Автоматический расчёт суммы доплаты триггером БД.
- Формирование отчётов по доплатам сотрудников.
- Регистрация и авторизация пользователей.

---

## 🛠 Технологический стек

| Компонент | Технология |
|-----------|------------|
| **Язык программирования** | C++11 |
| **Фреймворк GUI** | Qt 5 (Qt Widgets) |
| **СУБД** | PostgreSQL 15+ |
| **Архитектура** | MVC (Model-View-Controller) |
| **ORM-подход** | Ручное маппинг таблиц в C++ классы |
| **Безопасность** | SHA-256 хеширование паролей, параметрические SQL-запросы |

---

## 🗂 Структура базы данных

База данных спроектирована в **Нормальной форме Бойса-Кодда (BCNF)**. Схема включает 8 основных таблиц:

- `Роль` — справочник ролей пользователей.
- `List` — учётные записи пользователей.
- `Должность` — штатное расписание.
- `Квалификация` — квалификационные коэффициенты.
- `Сотрудник` — данные о работниках.
- `Вид_работы` — классификатор доп. работ с базовой оплатой.
- `Дополнительная_работа` — конкретные задачи.
- `Назначение` — связь сотрудников с задачами (с расчётом доплаты).

# Инфологическая модель:
<img width="1280" height="898" alt="Инфологическая модель" src="https://github.com/user-attachments/assets/a4792b27-2c9b-486c-bad8-ae8a2684f781" />

# Первичная реляциооная модель:
<img width="881" height="916" alt="Первичная_реляционная модель" src="https://github.com/user-attachments/assets/8ef80025-d914-4410-8133-ae102c260a20" />

# Реляционная модель:
<img width="1062" height="1047" alt="Реляционная модель" src="https://github.com/user-attachments/assets/bef5c81c-5550-42e6-bde8-f37e10c81abd" />

## 🧩 Архитектура приложения (MVC)
isrdo/
├── models/ # Классы-сущности (Employee, Work, Assignment...)
├── views/ # Qt-окна и диалоги (LoginWindow, MainWindow...)
├── controllers/ # Бизнес-логика (AuthController, WorkController...)
├── main.cpp # Точка входа
└── isrdo.pro # Сборка Qt

- **Model:** Отражение таблиц БД в C++ объектах.
- **View:** Интерфейсы на Qt, работающие через сигналы/слоты.
- **Controller:** Связующее звено, обрабатывает команды и обновляет модели.

---

## 🚀 Инструкция по запуску

### 1. Требования
- **OS:** Windows/Linux/macOS
- **Qt:** версия 5.15 или выше (с модулями `core`, `gui`, `widgets`, `sql`)
- **PostgreSQL:** 15 или выше
- **Компилятор:** GCC/MinGW или MSVC с поддержкой C++17

### 2. Настройка базы данных
1. Создайте базу данных в PostgreSQL (например, `kr_db`).
2. Выполните скрипт инициализации из файла [`database.sql`](./database.sql) (лежит в корне).
3. Проверьте настройки подключения в `main.cpp`:
   ```cpp
   dbm->openConnection("localhost", 5432, "kr_db", "postgres", "ваш_пароль");
3. Сборка приложения (Qt Creator)
Откройте файл проекта isrdo.pro в Qt Creator.

Настройте Kit под ваш компилятор.

Нажмите Собрать (Ctrl+B).

Запустите приложение (Ctrl+R).

# Итоговое приложение:
<img width="736" height="752" alt="PmogKtplkSpviWGYajTymTwSCqarEOnRa8L4XwSNbguXyZEdtpVjkLMKNc88xzyPQ7cEKEUG-T69SxAuaeQRqr7P" src="https://github.com/user-attachments/assets/13a6107e-d836-42f7-8c94-f21acb6746ea" />

<img width="1280" height="847" alt="NNaEyrk82Hxj-uS4LVSJgAtqPwz9Xh-YZqV9wrzwuoFWZrYA7CK0jnOsR1XGg6sHLMJ5I5S1nNpMagvjAG8cAZWT" src="https://github.com/user-attachments/assets/fca9d7cd-693f-4bd6-9bf2-c310283fbd25" />

<img width="928" height="779" alt="7_E3eo1wqWvSUV-41wtrw3R1XsYX2CaZBzJun2P-jDinvSp29ihpyTkziJLnIHeOdjkeJ-_d6f75g8zuP6wLHkjb" src="https://github.com/user-attachments/assets/2bd35bad-b33a-43a1-8166-951747864435" />

<img width="845" height="528" alt="LMqG3wm1djdD30oCtTsfuQYOvBZILC8xjlfa-oEQWzr5jotb0J0nonbKJk4iw-A5NIiAbknl09JLTOMtXmaDRp27" src="https://github.com/user-attachments/assets/209372f1-a837-464d-b3f0-090c3d9b52b4" />

<img width="1280" height="263" alt="7kwXZ1YDi_VoYqHJLfwPreIUm10ovr15XlWRgKib85gNzFQQ-xhqyVJ5KtHJDxdHbXjG25Fe2x9OQUiEieLd8nYh" src="https://github.com/user-attachments/assets/1f3c8bc6-1968-4695-be3b-ababff11694b" />
