#ifndef _QUESTION_H
#define _QUESTION_H

#include <array>
#include <string>

struct QuestionData
{
    std::string question;
    std::string answer;
    
    std::array<std::string, 4> options;
};

#endif