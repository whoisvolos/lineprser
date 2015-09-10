#include "LineParser.h"

LineParser::LineParser()
        : state(LP_STARTED),
          comment_re("^\\s{0,}\\#(.*)", regex_constants::icase),
          model_re("^\\s{0,}model\\s+([a-zA-Z0-9]+)$", regex_constants::icase),
          end_model_re("^\\s{0,}modelend", regex_constants::icase),
          frame_re("^\\s{0,}frame\\s+(\\d+)$", regex_constants::icase),
          end_frame_re("^\\s{0,}frameend.*$", regex_constants::icase),
          face_re("^\\s{0,}f\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)", regex_constants::icase),
          vertex_re("^\\s{0,}v\\s+([0-9\\-\\.]+)\\s+([0-9\\-\\.]+)\\s+([0-9\\-\\.]+)", regex_constants::icase),
          face_temp_re("^\\s{0,}f\\s+(\\d+)\\s+([0-9\\-\\.]+)$", regex_constants::icase),
          empty_re("^\\s{0,}$"),
          end_re("^\\s{0,}end.*$", regex_constants::icase),
          lineNum(0) { }

LineParser::~LineParser() {
    if (vertices.size() > 0) {
        vertices.clear();
    }
    if (faces.size() > 0) {
        faces.clear();
    }
}

int LineParser::onLine(string &line) {
    lineNum += 1;
    smatch match;

    // Empty string
    if (regex_match(line, match, empty_re)) {
        return 0;
    }

    // Comment
    if (regex_search(line, match, comment_re) && match.size() > 1) {
        string v = match.str(1);
        return onComment(v);
    }

    // Finished state
    if (state == LP_FINISHED) {
        error << "Parsing already completed";
        return 1;
    }

    if (state == LP_ERROR) {
        error << "Parsing failed";
        return 1;
    }

    if (regex_match(line, match, end_re)) {
        state = LP_FINISHED;
        return 1;
    }

    if (state == LP_STARTED) {
        if (regex_search(line, match, model_re) && match.size() > 1) {
            name = match.str(1);
            if (onModelStart(name) == 0) {
                state = LP_MODEL_START;
                return 0;
            } else {
                error << "Error in model start @ line #" << lineNum;
                state = LP_ERROR;
                return 1;
            }
        }
    }

    // Vetices, faces end model_end
    if (state == LP_MODEL_START) {
        // End of model
        // Clears model's data
        if (regex_search(line, match, end_model_re)) {
            if (onModelEnd(name, vertices, faces) == 0) {
                vertices.clear();
                faces.clear();
                state = LP_MODEL_END;
                return 0;
            } else {
                vertices.clear();
                faces.clear();
                error << "Error in model end @ line #" << lineNum;
                state = LP_ERROR;
                return 1;
            }
        }

        // New vertex
        if (regex_search(line, match, vertex_re)) {
            if (match.size() != 4) {
                vertices.clear();
                faces.clear();
                error << "Vertex takes exactly 3 parameters as floats @ line #" << lineNum;
                state = LP_ERROR;
                return 1;
            }
            vertices.push_back(stof(match.str(1), NULL));
            vertices.push_back(stof(match.str(2), NULL));
            vertices.push_back(stof(match.str(3), NULL));
            return 0;
        }

        // New face
        if (regex_search(line, match, face_re)) {
            if (match.size() != 4) {
                vertices.clear();
                faces.clear();
                error << "Face takes exactly 3 parameters as integers @ line #" << lineNum;
                state = LP_ERROR;
                return 1;
            }
            faces.push_back(stoi(match.str(1), NULL));
            faces.push_back(stoi(match.str(2), NULL));
            faces.push_back(stoi(match.str(3), NULL));
            return 0;
        }

        faces.clear();
        vertices.clear();
    }

    if (state == LP_MODEL_END) {
        if (regex_search(line, match, frame_re)) {
            if (match.size() != 2) {
                error << "Frame takes exactly 1 parameter as integer @ line #" << lineNum;
                state = LP_ERROR;
                return 1;
            }
            timestamp = stoul(match.str(1), NULL);
            temps.clear();
            if (onFrameStart(timestamp) != 0) {
                error << "Error in frame start @ line #" << lineNum;
                state = LP_ERROR;
                return 1;
            } else {
                state = LP_FRAME_START;
                return 0;
            }
        }
    }

    if (state == LP_FRAME_START) {
        if (regex_match(line, match, end_frame_re)) {
            if (onFrameEnd(timestamp, temps) == 0) {
                temps.clear();
                state = LP_MODEL_END;
                return 0;
            } else {
                temps.clear();
                error << "Error in frame end @ line #" << lineNum;
                state = LP_ERROR;
                return 1;
            }
        }
        if (regex_search(line, match, face_temp_re)) {
            if (match.size() != 3) {
                error << "Frame temperature takes exactly 2 parameters as integer and float @ line #" << lineNum;
                state = LP_ERROR;
                return 1;
            }
            int vn = stoi(match.str(1));
            float temp = stof(match.str(2));
            temps.push_back(pair<int, float>(vn, temp));
            return 0;
        }
    }

    error << "Unrecognized sequence @ line #" << lineNum;
    state = LP_ERROR;
    return 1;
}

int LineParser::onComment(string &comment) {
    return 0;
}

int LineParser::onModelStart(string &name) {
    cout << "Model \"" << name << "\" started" << endl;
    return 0;
}

int LineParser::onModelEnd(string &name, vector<float> &vertices, vector<uint> &faces) {
    cout << "Model \"" << name << "\" finished with " << vertices.size() / 3 << " vertices and " << faces.size() / 3 << " faces" << endl;
    return 0;
}

bool LineParser::isError() {
    return state == LP_ERROR;
}

string LineParser::getErrorStr() {
    return error.str();
}

void LineParser::reset() {
    lineNum = 0;
    vertices.clear();
    faces.clear();
    state = LP_STARTED;
    error.clear();
    temps.clear();
}

int LineParser::onFrameStart(unsigned long ms) {
    return 0;
}

int LineParser::onFrameEnd(unsigned long ms, vector<pair<int, float>> &temps) {
    cout << "Frame ended for ms = " << ms << endl;
    return 0;
}