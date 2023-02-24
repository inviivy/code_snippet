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

void test_util() {
  std::cout << ecs::TypeIndexGetter::Get<int>() << '\n';
  std::cout << ecs::TypeIndexGetter::Get<int>() << '\n';
  std::cout << ecs::TypeIndexGetter::Get<int>() << '\n';
  std::cout << ecs::TypeIndexGetter::Get<double>() << '\n';
  std::cout << ecs::TypeIndexGetter::Get<char>() << '\n';
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

  commands.Spawn(Name{"name"}, ID{123});
}

int main() {
  test_world();
  return 0;
}