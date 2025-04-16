#include<gtest.h>
#include"table.h"

TEST(TestUnorderedTable, can_insert) {
    UnorderedTable<std::string> table;
    table.insert(3, "three");
    table.insert(1, "one");
    table.insert(2, "two");
    EXPECT_EQ(table.find(3).second, "three");
    EXPECT_EQ(table.find(1).second, "one");
    EXPECT_EQ(table.find(2).second, "two");
    EXPECT_EQ(table.size(), 3);
}

TEST(TestUnorderedTable, can_erase) {
    UnorderedTable<std::string> table;
    table.insert(1, "one");
    table.insert(2, "two");
    table.insert(3, "three");

    EXPECT_TRUE(table.erase(2));
    EXPECT_EQ(table.size(), 2);
    EXPECT_THROW(table.find(2), std::logic_error);

    EXPECT_EQ(table.find(1).second, "one");
    EXPECT_EQ(table.find(3).second, "three");
}

TEST(TestUnorderedTable, copy_constr_is_correct) {
    UnorderedTable<std::string> table1;
    table1.insert(1, "one");
    table1.insert(2, "two");

    UnorderedTable<std::string> table2(table1);
    EXPECT_EQ(table2.size(), 2);
    EXPECT_EQ(table2.find(1).second, "one");
    EXPECT_EQ(table2.find(2).second, "two");
}

TEST(TestOrderedTable, binary_search_is_correct) {
    OrderedTable<std::string> table;
    table.insert(3, "three");
    table.insert(1, "one");
    table.insert(5, "five");
    table.insert(2, "two");
    table.insert(4, "four");

    EXPECT_EQ(table.find(1).second, "one");
    EXPECT_EQ(table.find(3).second, "three");
    EXPECT_EQ(table.find(5).second, "five");
    EXPECT_THROW(table.find(6), std::logic_error);
}

TEST(TestOrderedTable, can_insert) {
    OrderedTable<std::string> table;
    table.insert(3, "three");
    table.insert(1, "one");
    table.insert(2, "two");

    EXPECT_EQ(table.find(1).second, "one");
    EXPECT_EQ(table.find(2).second, "two");
    EXPECT_EQ(table.find(3).second, "three");
}

TEST(TestHashTable, can_create_hashtable) {
    ASSERT_NO_THROW(HashTable<std::string> table(1));
}

TEST(TestHashTable, can_insert) {
    HashTable<std::string> table(1);  
    table.insert(1, "one");
    table.insert(2, "two");
    table.insert(3, "three");

    EXPECT_EQ(table.size(), 3);
    EXPECT_EQ(table.find(1).second, "one");
    EXPECT_EQ(table.find(2).second, "two");
    EXPECT_EQ(table.find(3).second, "three");
}


TEST(TestHashTable, work_correctly_with_large_size) {
    HashTable<std::string> table;
    const size_t num_elements = 10000;

    for (size_t i = 0; i < num_elements; ++i) {
        table.insert(i, "value_" + std::to_string(i));
    }

    EXPECT_EQ(table.size(), num_elements);

    EXPECT_EQ(table.find(0).second, "value_0");
    EXPECT_EQ(table.find(5000).second, "value_5000");
    EXPECT_EQ(table.find(9999).second, "value_9999");

    for (size_t i = 0; i < num_elements; i += 2) {
        table.erase(i);
    }

    EXPECT_EQ(table.size(), num_elements / 2);
    EXPECT_EQ(table.find(1).second, "value_1");
}