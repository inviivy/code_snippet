#include "World.hpp"

#include <cassert>
#include <iostream>
#include <string>

struct Name {
  std::string name;
};

struct ID {
  int id;
};

struct Category {};
void test_util() {
  std::cout << ecs::TypeIndexGetter<Category>::Get<int>() << '\n';
  std::cout << ecs::TypeIndexGetter<Category>::Get<int>() << '\n';
  std::cout << ecs::TypeIndexGetter<Category>::Get<int>() << '\n';
  std::cout << ecs::TypeIndexGetter<Category>::Get<double>() << '\n';
  std::cout << ecs::TypeIndexGetter<Category>::Get<char>() << '\n';
  std::cout << "==============\n";
  std::cout << ecs::IdGenerator<uint32_t>::Gen() << '\n';
  std::cout << ecs::IdGenerator<uint32_t>::Gen() << '\n';
  std::cout << ecs::IdGenerator<uint32_t>::Gen() << '\n';
  std::cout << ecs::IdGenerator<uint32_t>::Gen() << '\n';
  std::cout << ecs::IdGenerator<uint32_t>::Gen() << '\n';
}

void test_sparset() {
  ecs::SparseSet<uint32_t, 32> set;
  set.Add(1);
  set.Add(2);
  set.Add(2);
  set.Add(3);

  assert(set.Contain(1) == true);
  assert(set.Contain(2) == true);
  assert(set.Contain(3) == true);
  assert(set.Contain(4) == false);
}

void test_world() {
  ecs::World world;
  ecs::Commands commands(world);

  commands.Spawn(Name{"name"}, ID{123}).Spawn(Name{std::string(20, 'c')});
}

struct Timer {
  int now;
};

void test_resource() {
  ecs::World world;
  ecs::Commands commands(world);

  commands.SetResource(Timer{1});
  commands.RemoveResource<Timer>();
}

struct NonType {};

void test_Query() {
  ecs::World world;
  ecs::Commands commands(world);
  ecs::Queryer queryer(world);
  commands.Spawn<Name>(Name{"abc"})
      .Spawn<Name>(Name{"def"})
      .Spawn<Name, ID>(Name{"xyz"}, ID{20})
      .Spawn<Name>(Name{std::string(20, 'c')});
  for (auto entity : queryer.Query<Name>()) {
    std::cout << queryer.Get<Name>(entity).name << '\n';
  }

  auto entities = queryer.Query<Name, NonType>();
  std::cout << entities.empty() << '\n';
}

void StartUpSystem(ecs::Commands command) {
  command.Spawn<Name>(Name{"A"})
      .Spawn<Name, ID>(Name{"B"}, ID{2})
      .Spawn<ID>(ID{3});
}

void EchoNameSystem(ecs::Commands command, ecs::Queryer query,
                    ecs::Resource resource) {
  std::cout << "echo name system\n";
  std::vector<ecs::Entity> entities = query.Query<Name>();
  for (auto entity : entities) {
    std::cout << query.Get<Name>(entity).name << '\n';
  }
}

void test_system() {
  std::cout << "===========>\n";
  ecs::World world;
  world.AddStartupSystem(StartUpSystem);
  world.AddSystem(EchoNameSystem);
  world.SetResource(Timer{1});
  world.Startup();
  world.Update();
}

int main() {
  test_world();
  test_resource();
  test_Query();
  test_system();
  return 0;
}