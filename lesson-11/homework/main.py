#! /usr/bin/env python3

import json
import ujson
import cjson
import time
import random
import string

def check():
    json_strs = [
        '{}',
        '{ "hello": 10, "world": "value" }',
        '{ "hello": 10 }',
        '{ "hello": 10, "hello": 10 }',
        '{ "hello1": 10, "hello2": 10 }',
        '{ "world": "value" }',
        '{ "world": "value", "world": "value" }',
        '{ "world1": "value", "world2": "value" }',
        '{ "hello": 1, "world2": "value", "!": 3 }',
        '{ "hello": "word1", "world": 2, "!": "word2" }',
        '{ "hello": -1, "world": -2, "!": -3 }',
    ]

    for i in range(len(json_strs)):
        input_str = json_strs[i]

        json_doc = json.loads(input_str)
        ujson_doc = ujson.loads(input_str)
        cjson_doc = cjson.loads(input_str)
        try:
            assert json_doc == ujson_doc == cjson_doc
        except AssertionError:
            print(f"ERROR IN TEST DOC: {i}")
            print(f"INPUT: {input_str}")
            print(f"{json_doc} != {cjson_doc} != {ujson_doc}")
            exit()

        json_str  =  json.dumps(json_doc)
        cjson_str = cjson.dumps(cjson_doc)
        try:
            assert json_str == cjson_str
        except AssertionError:
            print(f"ERROR IN TEST STR: {i}")
            print(f"INPUT: {input_str}")
            print(f"{json_str} != {cjson_str}")
            exit()

        print(f"TEST {i + 1} IS PASSED")
    print()

def check_performance():
    def create_large_random_input_str():
        print("GENERATING LARGE INPUT FOR PERFORMANCE TEST")
        pairs = dict()

        for i in range(10**7):
            key = str(i)

            p = random.random()
            if (p <= 0.5):
                value = random.randint(-1000000, 1000000)
            else:
                length = random.randint(1, 100)
                value = ''.join([random.choice(string.ascii_letters) for _ in range(length)])

            pairs[key] = value
        
        input_str = str(pairs)
        input_str = input_str.replace('\'', '\"')
        print("LARGE INPUT GENERATED")

        return input_str


    input_str =  create_large_random_input_str()
    print("PERFORMANCE TEST IS STARTED")

    start_ts = time.time()
    json_doc = json.loads(input_str)
    end_ts = time.time()
    print(f"Excetuion time of  loads: {end_ts - start_ts}")

    start_ts = time.time()
    ujson_doc = ujson.loads(input_str)
    end_ts = time.time()
    print(f"Excetuion time of uloads: {end_ts - start_ts}")

    start_ts = time.time()
    cjson_doc = cjson.loads(input_str)
    end_ts = time.time()
    print(f"Excetuion time of cloads: {end_ts - start_ts}")
    assert json_doc == cjson_doc == ujson_doc

    start_ts = time.time()
    json_str = json.dumps(json_doc)
    end_ts = time.time()
    print(f"Excetuion time of  dumps: {end_ts - start_ts}")

    start_ts = time.time()
    ujson_str = ujson.dumps(ujson_doc)
    end_ts = time.time()
    print(f"Excetuion time of udumps: {end_ts - start_ts}")

    start_ts = time.time()
    cjson_str = cjson.dumps(cjson_doc)
    end_ts = time.time()
    print(f"Excetuion time of cdumps: {end_ts - start_ts}")
    assert json_str == cjson_str


def main():
    check()
    check_performance()

if __name__ == "__main__":
    main()
