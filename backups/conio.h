char getch()
{
    char c;
    system("stty -echo");
    system("stty -icanon");
    
    c=getchar();
    
    system("stty icanon");
    system("stty echo");
    return c;
}

char getche()
{
    char c;
    system("stty -icanon");
    c=getchar();
    system("stty icanon");
    return c; 
}


