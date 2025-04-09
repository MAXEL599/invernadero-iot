import 'package:flutter/material.dart';
import 'principal.dart';
import 'login.dart';
import 'historial.dart';
import 'ajustes.dart';
import 'manuales.dart';
import 'configuracion_arduino.dart';
import 'registro.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter App',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
      ),
      initialRoute: '/',
      routes: {
        '/': (context) => const LoginPage(),
        '/principal': (context) => const PrincipalPage(),
        '/historial': (context) => const HistorialPage(),
        '/ajustes': (context) => const AjustesPage(),
        '/manuales': (context) => const ManualesPage(),
        '/configuracion_arduino': (context) => const ConfiguracionArduinoPage(),
        '/registro': (context) => const RegistroPage(),
      },
    );
  }
}
