#ifndef _QUESTION_H
#define _QUESTION_H

#include <array>
#include <string>

#define QUESTION_MAX_OPTIONS 4

struct QuestionData
{
    std::string question;
    std::string answer;
    
    std::array<std::string, QUESTION_MAX_OPTIONS> options;
    int option_count = 0; // how many options there are, 0 if its not MCQ
};

#endif