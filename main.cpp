
#include <iostream>
#include <map>
#include <memory>
#include <map>
#include <string>

using namespace std;

//------------------------------------------------------------------------------
/// Интерфейс объекта 
struct IObject
{
  virtual void print() = 0;
};
/// Прямоугольник
struct Rectangle : IObject
{
  void print() override { 
    cout << "  ----  " << endl; 
    cout << " |    | " << endl;
    cout << " |    | " << endl;
    cout << "  ----  " << endl;
  }
};
/// Круг
struct Circle : IObject
{
  void print() override { 
    cout << "   --   " << endl;
    cout << " /    \\ " << endl;
    cout << " \\    / " << endl;
    cout << "   --   " << endl;
  }
};

//------------------------------------------------------------------------------
/// получить имя со стандартного ввода
static string get_name()
{
  string name;
  bool first = true;
  do {
    cout << "Enter object name: ";
    if ( first ) {
      getline(std::cin, name);
      first = false;
    }
    getline(std::cin, name);
    //cout << "Object name: " << name << endl;
  } while ( name.empty() );
  return name;
}
/// создать объект
static std::pair<std::string, std::unique_ptr<IObject>> create_object() {
  // считываем тип
  int n = 0;
  do {
    cout << endl;
    cout << "--------------------------------------------" << endl;
    cout << 0 << " - exit" << endl;
    cout << 1 << " - rectangle" << endl;
    cout << 2 << " - circle" << endl;    
    cout << "--------------------------------------------" << endl;
    cout << endl << "Enter object type: ";
    cin >> n;
  } while ( n  < 0 || n > 2 );


  if ( n == 0 ) {
    return make_pair(string(), std::unique_ptr<IObject>());
  }
  // считываем имя
  std::unique_ptr<IObject> uptr;
  if ( n == 1 ) {
    uptr = std::unique_ptr<IObject>(new Rectangle);
  } else {
    uptr = std::unique_ptr<IObject>(new Circle);
  }

  return make_pair(get_name(), std::move(uptr));
}


//------------------------------------------------------------------------------
enum operation
{
  _create_doc = 2,
  _import_doc = 3,
  _export_doc = 4,
  _create_obj = 5,
  listall_obj = 6,
  _delete_obj = 7
};
/// интерфейс состояния конечного автомата
struct IProcOperation {
  /// Деструктор
  virtual ~IProcOperation() = default;
  /// образобать действие пользователя
  virtual IProcOperation *process(operation op) = 0;
  
  void print(const char *s) {
    cout << " --> " << s << endl;
  }
};
/// Состояние - пустой документ
class EmptyDocument : public IProcOperation
{
public:
  /// Конструктор
  EmptyDocument() = default;
  /// Деструктор
  ~EmptyDocument() = default;

  /// образобать действие пользователя
  IProcOperation *process(operation op) override;
};
/// Состояние - не пустой документ (содержет сколько-то объектов)
class DocumentWithObject : public IProcOperation
{
public:
  using objects_t = std::map<std::string, std::unique_ptr<IObject>>;
  /// Конструктор
  DocumentWithObject(objects_t && d) : m_objects(std::move(d)) {} ;
  /// Конструктор
  DocumentWithObject() = default;
  /// Деструктор
  ~DocumentWithObject() = default;

  /// образобать действие пользователя
  IProcOperation *process(operation op) override;

  /// Добавить объект
  bool insert(std::pair<std::string, std::unique_ptr<IObject>> data) {
    if ( data.second ) {
      auto ret = m_objects.insert(std::move(data));
      if ( ret.second ) {        
        return true;
      } else {
        cout << "object with this name is exist already" << endl;
      }
    }
    return false;
  }
  /// удалить объект
  bool delete_obj() {
    auto it = m_objects.find(get_name());
    if ( it == m_objects.end() ) {
      cout << "object with this name does not exist" << endl;
      return false;
    }
    m_objects.erase(it);
    return true;
  }

private:
  objects_t m_objects;
};


IProcOperation *EmptyDocument::process(operation op)
{
  switch ( op ) {
  case _create_doc:
    print("already a new document");
    return new EmptyDocument();
    break;
  case _import_doc:
    print("doc was imported");
    return new EmptyDocument();
    break;
  case _export_doc:
    print("this is an empty doc");
    return new EmptyDocument();
    break;
  case _create_obj: {
      DocumentWithObject* ptr = new DocumentWithObject();
      if ( ptr->insert(create_object()) ) {
        print("object was created");
        return ptr;
      }
      delete ptr;
      return new EmptyDocument();
    }
    break;
  case listall_obj:
    print("this is an empty doc");
    return new EmptyDocument();
    break;
  case _delete_obj:
    print("this is an empty doc");
    return new EmptyDocument();
    break;
  default:
    break;
  }
  return nullptr;
}

IProcOperation *DocumentWithObject::process(operation op)
{
  switch ( op ) {
  case _create_doc:
    print("a new doc was created");
    return new EmptyDocument();
    break;
  case _import_doc:
    print("doc was imported");
    return new EmptyDocument();
    break;
  case _export_doc:
    print("doc was exported");
    return new DocumentWithObject(std::move(m_objects));
    break;
  case _create_obj:
    {      
      if ( insert(create_object()) ) {
        print("object was created");
      }     
      return new DocumentWithObject(std::move(m_objects));
    }
    break;
  case listall_obj:
    for (auto &d: m_objects ) {
      cout << d.first << endl;
      d.second->print();
    }
    return new DocumentWithObject(std::move(m_objects));
    break;
  case _delete_obj:
    if ( delete_obj())
      print("object was deleted");
    if ( m_objects.empty())
      return new EmptyDocument();
    return new DocumentWithObject(std::move(m_objects));
    break;
  default:
    break;
  }
  return nullptr;
}
//------------------------------------------------------------------------------












int main(int argc, char const *argv[])
{
  int n = 0;
  IProcOperation *interface_ = new EmptyDocument();

  do {
    static bool print_op = true;    
    if ( print_op ) {
      cout << endl;
      cout << "--------------------------------------------" << endl;
      cout << 0 << " - exit" << endl;
      cout << 1 << " - list the operations" << endl;
      cout << _create_doc << " - create a new document" << endl;
      cout << _import_doc << " - import the document"   << endl;
      cout << _export_doc << " - export the document"   << endl;
      cout << _create_obj << " - create an object"      << endl;
      cout << listall_obj << " - list all the objects"  << endl;
      cout << _delete_obj << " - delete an object"      << endl;
      cout << "--------------------------------------------" << endl;
      print_op = false;
    }
    cout << endl << "Enter operation: ";
    
    cin >> n;

    if ( n == 1 ) {
      print_op = true;
    }

    if ( n > 1 ) {
      IProcOperation *tmp = interface_->process((operation) n);
      delete interface_;
      interface_ = tmp;
    }

  } while ( n!= 0 );

  delete interface_;

	return 0;
}
//------------------------------------------------------------------------------