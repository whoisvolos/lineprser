#pragma once

#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <sstream>

using namespace std;

class LineParser {
private:
    typedef enum {
        LP_STARTED,
        LP_MODEL_START,
        LP_MODEL_END,
        LP_FRAME_START,
        LP_FINISHED,
        LP_ERROR
    } lpState;

    lpState state;
    regex comment_re,
            model_re,
            end_model_re,
            frame_re,
            end_frame_re,
            face_re,
            vertex_re,
            face_temp_re,
            empty_re,
            end_re;

    vector<float> vertices;
    vector<uint> faces;
    string name;
    int lineNum;
    stringstream error;

    unsigned long timestamp;
    vector<pair<int, float>> temps;

public:
    LineParser();
    ~LineParser();

    int onLine(string& line);

    bool isError();
    string getErrorStr();
    void reset();

    virtual int onComment(string& comment);
    virtual int onModelStart(string &name);
    virtual int onModelEnd(string &name, vector<float>& vertices, vector<uint>& faces);
    virtual int onFrameStart(unsigned long ms);
    virtual int onFrameEnd(unsigned long ms, vector<pair<int, float>>& temps);
};