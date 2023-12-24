 //С++ и БД. ORM.cpp 


#include <iostream>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>
#include <windows.h>
#pragma execution_character_set("utf-8")

class Publisher;
class Book;
class Shop;
class Stock;
class Sale;


class Book {
public:
    std::string title = "";    
    Wt::Dbo::ptr<Publisher> publisher;
    Wt::Dbo::collection< Wt::Dbo::ptr<Stock> > stock;

    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, title, "title");        
        Wt::Dbo::belongsTo(a, publisher, "publisher");
        Wt::Dbo::hasMany(a, stock, Wt::Dbo::ManyToOne, "books");
    }
};

class Publisher {
public:
    std::string name = "";
    Wt::Dbo::collection< Wt::Dbo::ptr<Book> > books;

    

    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::hasMany(a, books, Wt::Dbo::ManyToOne, "publisher");

    }
};

class Shop {
public:
    std::string name = "";
    Wt::Dbo::collection< Wt::Dbo::ptr<Stock> > stock;   

    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::hasMany(a, stock, Wt::Dbo::ManyToOne, "shop");        
    }
};

class Stock {
public:
    int count = 0;

    Wt::Dbo::ptr<Book> books;
    Wt::Dbo::ptr<Shop> shop;
    Wt::Dbo::collection< Wt::Dbo::ptr<Sale> > sale;

    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, count, "count");
        Wt::Dbo::belongsTo(a, books, "books");
        Wt::Dbo::belongsTo(a, shop, "shop");
        Wt::Dbo::hasMany(a, sale, Wt::Dbo::ManyToOne, "stock");
    }
};

class Sale {
public:
    int count = 0;
    int prace = 0;
    std::string date_sale;

    Wt::Dbo::ptr<Stock> stock;
    

    template<class Action>
    void persist(Action& a)
    {
        Wt::Dbo::field(a, count, "count");
        Wt::Dbo::field(a, prace, "prace");
        Wt::Dbo::field(a, date_sale, "date_sale");
        Wt::Dbo::belongsTo(a, stock, "stock");
       
    }
};


int main()
{
    //setlocale(LC_ALL, "ru");
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    try {
        std::string connectionString =
            "host =localhost "
            "port = 5432 "
            "dbname = postgres "
            "user = postgres "
            "password=mjgojwx6$FGHc985khKKLO";

        std::cout << "подключение" << std::endl;

        auto postgres = std::make_unique<Wt::Dbo::backend::Postgres>(connectionString);
        Wt::Dbo::Session session;
        session.setConnection(std::move(postgres));

        

        session.mapClass<Publisher>("publisher");
        session.mapClass<Book>("books");
        session.mapClass<Shop>("shop");
        session.mapClass<Stock>("stock");
        session.mapClass<Sale>("sale");
         
        try
        {
            session.dropTables();
        }
        catch (const Wt::Dbo::Exception& e)
        {
            std::cout << e.what() << std::endl;
        }
         
         try
         {
             session.createTables();
         }
         catch (const Wt::Dbo::Exception& e)
         {
             std::cout << e.what() << std::endl;
         }

         try
         {
             Wt::Dbo::Transaction transaction{ session };

             std::unique_ptr<Publisher> publisher1(new Publisher{ "Alpina" });
             auto publisher1db = session.add<Publisher>(std::move(publisher1));

             std::unique_ptr<Book> books(new Book{ "The shorter MBA" });
             auto booksdb = session.add<Book>(std::move(books));
             booksdb.modify()->publisher = publisher1db;

             /*std::unique_ptr<Book> books(new Book{ "Cast" });
             auto booksdb = session.add<Book>(std::move(books));
             booksdb.modify()->publisher = publisher1db;*/

             std::unique_ptr<Shop> shop1(new Shop{ "Книги" });
             auto shop1db = session.add<Shop>(std::move(shop1));

             std::unique_ptr<Stock> stock1(new Stock{ 3 });
             auto stock1db = session.add<Stock>(std::move(stock1));
             stock1db.modify()->books = booksdb;
             stock1db.modify()->shop = shop1db;

             std::unique_ptr<Sale> sale1(new Sale{ 2,4,"2023-11-25" });
             auto sale1db = session.add<Sale>(std::move(sale1));
             sale1db.modify()->stock = stock1db;

             transaction.commit(); 
         }
         catch (const Wt::Dbo::Exception& e)
         {
             std::cout << e.what() << std::endl;
         }

        

        std::cout << "enter publisher_id:"<< std::endl;
        int pub;
        std::cin >> pub; 

        Wt::Dbo::Transaction transaction1{ session };

  Wt::Dbo::collection< Wt::Dbo::ptr<Book>> books = session.find<Book>().where("publisher_id = ?").bind(pub);
  
            for (const Wt::Dbo::ptr<Book>& book : books)
        {                
                Wt::Dbo::collection< Wt::Dbo::ptr<Stock>> stock = session.find<Stock>().where("books_id = ?").bind(book.id());
                for (const Wt::Dbo::ptr<Stock>& stock : stock)
                {
                    std::string shops = stock->shop->name;
                    std::cout<<"the stores where the publisher books: " << shops << std::endl;
                }
        }            

        transaction1.commit();
    }


    catch (const Wt::Dbo::Exception& e)
    {
        std::cout << e.what() << std::endl;
    }    
        
    return 0;
}









